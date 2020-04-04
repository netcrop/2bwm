pkgname='2bwm'
pkgver=0.2
pkgrel=3
pkgdesc="A fast floating window manager"
arch=('x86_64')
url="https://github.com/netcrop/2bwm"
license=('ISC')
depends=('xcb-util-wm' 'xcb-util-xrm' 'xcb-util-keysyms')
makedepends=('git' 'xcb-util')
source=("$pkgname::git+https://github.com/netcrop/2bwm.git")
md5sums=('SKIP')

build() {
  cd $srcdir/
  make PREFIX=/usr
}
package() {
  cd $srcdir/
  make DESTDIR="$pkgdir" PREFIX=/usr MANPREFIX=/usr/share/man install
}
