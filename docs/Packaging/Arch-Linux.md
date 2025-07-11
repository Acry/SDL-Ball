# Arch-Linux-Packaging

Für Arch Linux empfiehlt es sich, eine `PKGBUILD`-Datei zu erstellen. Diese Datei beschreibt, wie das Paket gebaut und
installiert wird. Hier ein einfaches Beispiel für dein Projekt

```bash
# Maintainer: Dein Name <deine@email.de>
pkgname=sdl-ball-remastered
pkgver=1.0.0
pkgrel=1
pkgdesc="Remaster von SDL-Ball"
arch=('x86_64')
url="https://github.com/Acry/SDL-Ball"
license=('GPL')
depends=('sdl2' 'sdl2_image' 'sdl2_ttf' 'sdl2_mixer' 'epoxy')
makedepends=('clang' 'make')
source=("$pkgname-$pkgver.tar.gz::https://github.com/DEIN_GITHUB/sdl-ball-remastered/archive/refs/tags/v$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$srcdir/$pkgname-$pkgver"
    make PREFIX=/usr DATADIR=/usr/share/sdl-ball-remastered/themes/
}

package() {
    cd "$srcdir/$pkgname-$pkgver"
    make DESTDIR="$pkgdir/" PREFIX=/usr DATADIR=/usr/share/sdl-ball-remastered/themes/ install
}
```

## Notes

Die Abhängigkeiten (depends) müssen ggf. erweitert werden.
Die Variable DATADIR wird wie im Makefile empfohlen gesetzt.
Die Installation erfolgt ins richtige Verzeichnis mit DESTDIR.
Diese Datei kommt ins Wurzelverzeichnis deines Quellcodes oder in ein separates Build-Verzeichnis.
Das Paket baust du dann mit makepkg.
