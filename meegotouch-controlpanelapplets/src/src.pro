TEMPLATE = subdirs

include(../shared.pri)
include(../localconfig.pri)

######if no BUILD_APPLET given, we add all of the subdirs, 
###### otherwise we just add the selected applets subdirs

isEmpty(BUILD_APPLET) {
message ("*********No BUILD_APPLET given, build only the Harmattan applets*************")

addSubDirs(aboutapplet)
addSubDirs(aboutapplet/translations)
addSubDirs(libmeegocontrol)
addSubDirs(batteryapplet)
addSubDirs(batteryapplet/translations)
addSubDirs(displayapplet)
addSubDirs(displayapplet/translations)
addSubDirs(offlineapplet)
addSubDirs(offlineapplet/translations)
addSubDirs(resetapplet)
addSubDirs(resetapplet/translations)
addSubDirs(soundsettingsapplet)
addSubDirs(soundsettingsapplet/translations)
addSubDirs(testapplet)
addSubDirs(testapplet/translations)
addSubDirs(usbapplet)
addSubDirs(usbapplet/translations)
addSubDirs(wallpaperapplet2)
addSubDirs(wallpaperapplet2/translations)
addSubDirs(warrantyapplet)
}

if (BUILD_APPLET)
{
contains(BUILD_APPLET,about){
message ("*************build aboutapplet********************")
addSubDirs(aboutapplet)
addSubDirs(aboutapplet/translations)
}
contains (BUILD_APPLET, theme){
message ("************build themeapplet*********************")
addSubDirs(themeapplet)
addSubDirs(themeapplet/translations)
}
contains (BUILD_APPLET,libmeegocontrol ){
message ("***********build libmeegocon *********************")
addSubDirs(libmeegocontrol)
}
contains (BUILD_APPLET, battery){
message ("***********build battery *************************")
addSubDirs(batteryapplet)
addSubDirs(batteryapplet/translations)
}
contains (BUILD_APPLET,display ){
message ("***********build display *************************")
addSubDirs(displayapplet)
addSubDirs(displayapplet/translations)
}
contains (BUILD_APPLET,gprs ){
message ("***********build gprs *************************")
addSubDirs(gprsapplet)
addSubDirs(gprsapplet/translations)
}
contains (BUILD_APPLET, offline){
message ("***********build offline *************************")
addSubDirs(offlineapplet)
addSubDirs(offlineapplet/translations)
}
contains (BUILD_APPLET,reset ){
message ("***********build reset ***************************")
addSubDirs(resetapplet)
addSubDirs(resetapplet/translations)
}
contains (BUILD_APPLET, sound){
message ("***********build soundsettings *******************")
addSubDirs(soundsettingsapplet)
addSubDirs(soundsettingsapplet/translations)
}
contains (BUILD_APPLET,testapplet ){
message ("***********build testapplet *********************")
addSubDirs(testapplet)
addSubDirs(testapplet/translations)
}
contains (BUILD_APPLET,usb){
message ("***********build usb ****************************")
addSubDirs(usbapplet)
addSubDirs(usbapplet/translations)
}
contains (BUILD_APPLET,wallpaper ){
message ("***********build wallpaper **********************")
addSubDirs(wallpaperapplet)
addSubDirs(wallpaperapplet/translations)
}
contains (BUILD_APPLET,wallpaper2 ){
message ("***********build wallpaper2 *********************")
addSubDirs(wallpaperapplet2)
addSubDirs(wallpaperapplet2/translations)
}
contains (BUILD_APPLET, warranty){
message ("***********build warranty ***********************")
addSubDirs(warrantyapplet)
addSubDirs(warrantyapplet/translations)
}
}

