!include(zayeditor.pri) {
    error("BOSS2D : Couldn't find the zayeditor.pri file...")
}

win32-msvc*{
	message("BOSS2D : The platform was decided to be win32-msvc*...")
} else {
win32-g++{
	message("BOSS2D : The platform was decided to be win32-g++...")
} else {
linux-g++{
	message("BOSS2D : The platform was decided to be linux-g++...")
} else {
macx{
	message("BOSS2D : The platform was decided to be macx...")
} else {
ios{
	message("BOSS2D : The platform was decided to be ios...")
} else {
android{
	message("BOSS2D : The platform was decided to be android...")
} else {
wasm{
	message("BOSS2D : The platform was decided to be wasm...")
} else {
	error("BOSS2D : You can not build on this platform!")
} #else wasm
} #else android
} #else ios
} #else macx
} #else linux-g++
} #else win32-g++
} #else win32-msvc
