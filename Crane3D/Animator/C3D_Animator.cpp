# include "C3D_Animator.hpp"
# include "../Define/C3D_ModelData.hpp"

namespace
{

	static Float3 CalcVector3(const Array<crane::VectorKey>& keys, const double time, const Float3& def)
	{
		if (keys.isEmpty())
		{
			return def;
		}
		if (keys.size() == 1)
		{
			return keys.front().value;
		}

		size_t nextIdx = 0;
		while (nextIdx < keys.size())
		{
			if (time < keys[nextIdx].time)
			{
				break;
			}
			++nextIdx;
		}

		if (nextIdx == 0)
		{
			return keys.front().value;
		}
		if (nextIdx == keys.size())
		{
			return keys.back().value;
		}

		const auto& prev = keys[nextIdx - 1];
		const auto& next = keys[nextIdx];

		const double diff = next.time - prev.time;
		if (diff <= 0.0)
		{
			return prev.value;
		}
		const float t = static_cast<float>((time - prev.time) / diff);
		return prev.value.lerp(next.value, t);
	}

	static Quaternion CalcQuaternion(const Array<crane::QuaternionKey>& keys, const double time, const Quaternion& def)
	{
		if (keys.isEmpty())
		{
			return def;
		}
		if (keys.size() == 1)
		{
			return keys.front().value;
		}

		size_t nextIdx = 0;
		while (nextIdx < keys.size())
		{
			if (time < keys[nextIdx].time)
			{
				break;
			}
			++nextIdx;
		}
		if (nextIdx == 0)
		{
			return keys.front().value;
		}
		if (nextIdx == keys.size())
		{
			return keys.back().value;
		}

		const auto& prev = keys[nextIdx - 1];
		const auto& next = keys[nextIdx];

		const double diff = next.time - prev.time;
		if (diff <= 0.0)
		{
			return prev.value;
		}
		const float t = static_cast<float>((time - prev.time) / diff);
		return prev.value.slerp(next.value, t);
	}
}

namespace crane
{

	bool Animator::setup(const crane::ModelData* pModelData)
	{
		if (pModelData == nullptr)
		{
			return false;
		}
		m_pModelData = pModelData;

		std::fill(
			std::begin(m_boneDataRaw.boneMatrices)
			, std::end(m_boneDataRaw.boneMatrices)
			, Mat4x4::Identity()
		);
		m_nodeGlobalTransforms.resize(m_pModelData->nodes.size(), Mat4x4::Identity());

		return true;
	}

	void Animator::changeClip(
		const size_t index
		, const double fadeDuration
		, const PlayMode mode
		, const SameOverride sameOverride
	)
	{
		if (m_pModelData == nullptr
			|| m_pModelData->animationClips.size() <= index)
		{
			return;
		}
		if (not sameOverride)
		{
			if (m_optNextClip && m_optNextClip.value() == index)
			{
				return;
			}
			if (m_clipIndex == index)
			{
				return;
			}
		}

		if (0.0 < fadeDuration)
		{
			m_optNextClip = index;
			m_nextTime = 0.0;
			m_nextPlayMode = mode;
			m_fadeDuration = fadeDuration;
			m_fadeTimer = 0.0;
		}
		else
		{
			m_clipIndex = index;
			m_playMode = mode;
			m_currentTime = 0.0;
			m_optNextClip.reset();
		}
	}

	void Animator::update(const double deltaTime)
	{
		if (m_pModelData == nullptr
			|| m_pModelData->animationClips.size() <= m_clipIndex)
		{
			return;
		}

		const auto& clip = m_pModelData->animationClips[m_clipIndex];
		m_currentTime += deltaTime;


		m_isAnimationEnd = false;
		double animTime = m_currentTime * clip.ticksPerSecond;
		if (m_playMode == PlayMode::Loop)
		{
			animTime = Math::Fmod(animTime, clip.duration);
		}
		else if (clip.duration <= animTime)
		{
			animTime = clip.duration;
			m_isAnimationEnd = true;
		}

		double nextAnimTime = 0.0;
		float blendWeight = 0.0f;
		const AnimationClip* pNextClip = nullptr;
		if (m_optNextClip)
		{
			pNextClip = &(m_pModelData->animationClips[(*m_optNextClip)]);
			m_nextTime += deltaTime;
			nextAnimTime = m_nextTime * pNextClip->ticksPerSecond;

			if (m_nextPlayMode == PlayMode::Loop)
			{
				nextAnimTime = Math::Fmod(nextAnimTime, pNextClip->duration);
			}
			else if (pNextClip->duration <= nextAnimTime)
			{
				nextAnimTime = pNextClip->duration;
			}

			m_fadeTimer += deltaTime;
			blendWeight = static_cast<float>(Math::Clamp(m_fadeTimer / m_fadeDuration, 0.0, 1.0));
			m_isAnimationEnd = false;
		}

		if (not m_pModelData->nodes.isEmpty())
		{
			updateNodeHierarchy(0, Mat4x4::Identity(), animTime, clip, nextAnimTime, pNextClip, blendWeight);
		}

		if (m_optNextClip && m_fadeDuration <= m_fadeTimer)
		{
			m_clipIndex = m_optNextClip.value();
			m_currentTime = m_nextTime;
			m_playMode = m_nextPlayMode;
			animTime = nextAnimTime;
			m_optNextClip.reset();
		}
	}

	const CBVSBoneData& Animator::getBoneData() const
	{
		return m_boneDataRaw;
	}

	Optional<Mat4x4> Animator::getSocketTransform(const NameHash nodeNameHash) const
	{
		if (m_pModelData == nullptr)
		{
			return none;
		}

		const auto& it = m_pModelData->nodeIndexTable.find(nodeNameHash);
		if (it != m_pModelData->nodeIndexTable.end())
		{
			return m_nodeGlobalTransforms[it->second];
		}
		return none;
	}

	bool Animator::isAnimationEnd() const
	{
		return m_isAnimationEnd;
	}

	double Animator::getProgress() const
	{
		if (m_pModelData == nullptr
			|| m_pModelData->animationClips.size() <= m_clipIndex)
		{
			return 0.0;
		}

		const auto& clip = m_pModelData->animationClips[m_clipIndex];
		if (clip.duration <= 0.0)
		{
			return 0.0;
		}

		const double animTime = m_currentTime * clip.ticksPerSecond;
		return Math::Clamp(animTime / clip.duration, 0.0, 1.0);
	}

	double Animator::getCurrentTime() const
	{
		return m_currentTime;
	}

	void Animator::updateNodeHierarchy(
		const uint32 nodeIndex
		, const Mat4x4& parentMat
		, const double animTime
		, const AnimationClip& clip
		, const double nextAnimTime
		, const AnimationClip* pNextClip
		, const float blendWeight
		)
	{
		const NodeData& node = m_pModelData->nodes[nodeIndex];
		Mat4x4 localMat = node.localTransform;

		const AnimationChannel* pChannel = nullptr;
		for (const auto& ch : clip.channels)
		{
			if (ch.nodeNameHash == node.nameHash)
			{
				pChannel = &ch;
				break;
			}
		}

		const AnimationChannel* pNextChannel = nullptr;
		if (pNextClip)
		{
			for (const auto& ch : pNextClip->channels)
			{
				if (ch.nodeNameHash == node.nameHash)
				{
					pNextChannel = &ch;
					break;
				}
			}
		}

		if (pChannel != nullptr)
		{
			Float3 defScale, defPos;
			Quaternion defRot;
			node.localTransform.decompose(defScale, defRot, defPos);

			auto scale = CalcVector3(pChannel->scaleKeys, animTime, defScale);
			auto rot = CalcQuaternion(pChannel->rotationKeys, animTime, defRot);
			auto pos = CalcVector3(pChannel->positionKeys, animTime, defPos);

			if (pNextClip)
			{
				auto nextScale = defScale;
				auto nextRot = defRot;
				auto nextPos = defPos;
				if (pNextChannel != nullptr)
				{
					nextScale = CalcVector3(pNextChannel->scaleKeys, nextAnimTime, defScale);
					nextRot = CalcQuaternion(pNextChannel->rotationKeys, nextAnimTime, defRot);
					nextPos = CalcVector3(pNextChannel->positionKeys, nextAnimTime, defPos);
				}
				scale = scale.lerp(nextScale, blendWeight);
				rot = rot.slerp(nextRot, blendWeight);
				pos = pos.lerp(nextPos, blendWeight);
			}

			localMat = Mat4x4::Scale(scale) * Mat4x4::Rotate(rot) * Mat4x4::Translate(pos);
		}

		const Mat4x4 globalMat = localMat * parentMat;
		m_nodeGlobalTransforms[nodeIndex] = globalMat;

		const auto it = m_pModelData->boneIndexTable.find(node.nameHash);
		if (it != m_pModelData->boneIndexTable.end())
		{
			const int32 boneIndex = it->second;
			const auto& bone = m_pModelData->bones[boneIndex];

			m_boneDataRaw.boneMatrices[boneIndex] = bone.offsetMatrix * globalMat;
		}

		for (const uint32 idx : node.childrenIndices)
		{
			updateNodeHierarchy(idx, globalMat, animTime, clip, nextAnimTime, pNextClip, blendWeight);
		}
	}

}
