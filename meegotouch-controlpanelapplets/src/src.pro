TEMPLATE = subdirs

include(../shared.pri)
include(../localconfig.pri)

######if no BUILD_APPLET given, we add all of the subdirs, 
###### otherwise we just add the selected applets subdirs

isEmpty(BUILD_APPLET) {
message ("*********No BUILD_APPLET given, build only the Harmattan applets*************")

addSubDirs(aboutapplet)
addSubDirs(aboutapplet/translations)
# Away from the N9: the sound settings (and libmeegocontrol, which is
# made of them) need libprofile and GStreamer 0.10, the wallpaper applet
# the thumbnailer; they are built where those are found
contains(DEFINES, HAVE_LIBPROFILE): addSubDirs(libmeegocontrol)
# needs QmSystem2
contains(DEFINES, HAVE_QMSYSTEM) {
addSubDirs(batteryapplet)
addSubDirs(batteryapplet/translations)
}
# the display applet needs QmSystem2 (display state, brightness)
contains(DEFINES, HAVE_QMSYSTEM) {
addSubDirs(displayapplet)
addSubDirs(displayapplet/translations)
}
# needs QmSystem2
contains(DEFINES, HAVE_QMSYSTEM) {
addSubDirs(offlineapplet)
addSubDirs(offlineapplet/translations)
}
addSubDirs(resetapplet)
addSubDirs(resetapplet/translations)
contains(DEFINES, HAVE_LIBPROFILE) {
addSubDirs(soundsettingsapplet)
addSubDirs(soundsettingsapplet/translations)
}
addSubDirs(testapplet)
addSubDirs(testapplet/translations)
# needs QmSystem2
contains(DEFINES, HAVE_QMSYSTEM) {
addSubDirs(usbapplet)
addSubDirs(usbapplet/translations)
}
exists(/usr/include/thumbnailer) {
addSubDirs(wallpaperapplet2)
addSubDirs(wallpaperapplet2/translations)
}
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

