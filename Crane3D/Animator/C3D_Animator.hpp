# pragma once
# include "../Define/C3D_Define.hpp"
# include "../Define/C3D_CBVSBoneData.hpp"

namespace crane
{
	/// @brief アニメーション再生時の指定
	enum class PlayMode
	{
		OneShot, ///< 1回のみ
		Loop, ///< ループ再生
	};

	/// @brief アニメーション再生時の同じアニメーション指定で最初から再生するかどうかのYES/NO
	using SameOverride = YesNo<struct SameOverride_tag>;

	struct ModelData;
	struct AnimationClip;

	class Animator
	{
	public:

		Animator() = default;

		/// @brief データの初期化などを含むセットアップを行います。
		/// @param pModelData 参照するモデルデータ
		/// @return true: 成功, false: 失敗
		bool setup(const crane::ModelData* pModelData);

		/// @brief アニメーションクリップを切り替えます。
		/// @param index 切り替え先
		/// @param fadeDuration 切り替えまでの補間時間(秒)
		/// @param mode 再生するクリップの繰り返し設定(Loop or OneShot)
		/// @param sameOverride 同じ切り替え先を指定した場合、最初から再生するかどうか
		void changeClip(
			const size_t index, const double fadeDuration
			, const PlayMode mode, const SameOverride sameOverride
		);

		/// @brief アニメーションの更新
		/// @param deltaTime 更新時間（Scene::DeltaTime()などが指定される想定。）
		void update(const double deltaTime);

		/// @brief コンスタントバッファ用ボーン情報を取得します。
		/// @return 計算済みボーン情報
		const CBVSBoneData& getBoneData() const;

		/// @brief 指定したノードのグローバルマトリクスを取得します。
		/// @param nodeNameHash 取得したいノードのハッシュ
		/// @return グローバルマトリクス、存在しなければnoneが帰ります。
		Optional<Mat4x4> getSocketTransform(const NameHash nodeNameHash) const;

		/// @brief アニメーションが終了しているかを取得します。Loopを指定するとtrueになることはありません。
		/// @return true: 終了, false: 再生中
		bool isAnimationEnd() const;

		/// @brief 再生しているアニメーションの進捗率（0.0 ~ 1.0）を取得します。
		/// @return 再生しているアニメーションの進捗率（0.0 ~ 1.0）
		double getProgress() const;

		/// @brief 再生しているアニメーションの時間（秒）を取得します。
		/// @return 再生しているアニメーションの時間（秒）
		double getCurrentTime() const;

	private:

		const ModelData* m_pModelData{ nullptr };
		size_t m_clipIndex{ 0U };
		Optional<size_t> m_optNextClip{ none };
		double m_currentTime{ 0.0 };
		double m_nextTime{ 0.0 };
		double m_fadeTimer{ 0.0 };
		double m_fadeDuration{ 0.0 };
		PlayMode m_playMode{ PlayMode::Loop };
		PlayMode m_nextPlayMode{ PlayMode::Loop };
		CBVSBoneData m_boneDataRaw{};
		Array<Mat4x4> m_nodeGlobalTransforms{};
		bool m_isAnimationEnd{ false };

		void updateNodeHierarchy(
			const uint32 nodeIndex
			, const Mat4x4& parentMat
			, const double animTime
			, const AnimationClip& clip
			, const double nextAnimTime
			, const AnimationClip* pNextClip
			, const float blendWeight
		);

	};
}
