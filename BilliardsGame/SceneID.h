/* シーンIDリスト */
// SceneManagerでのシーン切り替えで使われるリスト
// 列挙体の定義のみ有ればいいケースがあるのでファイルを分けている

#pragma once

enum class SceneID
{
	Title,			// タイトル
	G_NineBall,		// メインゲーム(ナインボール)
	G_Rotation,		// メインゲーム(ローテーション)
	Exit,			// ゲームを終了
	Keep,			// シーンはそのまま
	Reset,			// シーンを再生成する
};