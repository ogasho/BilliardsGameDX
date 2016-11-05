/* シーンIDリスト */
// SceneManagerでのシーン切り替えで使われるリスト
// 列挙体の定義のみ有ればいいケースがあるのでファイルを分けている

#pragma once

enum class SceneID
{
	Title,			// タイトル
	G_NineBall,		// メインゲーム
	Exit,			// ゲームを終了
	Keep,			// シーンはそのまま
};