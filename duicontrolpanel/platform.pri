isEmpty(HOST_ARCH) {

# the Harmattan device build (libsysinfo) only where its header is:
# on Debian (Leste) dpkg-architecture alone does not mean Harmattan
HOST_ARCH=$$system(dpkg-architecture -qDEB_HOST_GNU_TYPE || echo meego)
!exists(/usr/include/sysinfo.h): HOST_ARCH = meego
message("arch $$HOST_ARCH")
contains(HOST_ARCH,meego){
  message("preparing for MEEGO")
  CONFIG += meego
  DEFINES += MEEGO
}

meego {
  message("MEEGO")
} else {
message("NON-MEEGO")
}

}

