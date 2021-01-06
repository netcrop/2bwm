2bwm.substitute()
{
    local cmdlist reslist devlist pkglist cmd i pkg
    cmdlist=(dirname basename cat ls mv sudo cp chmod ln chown rm touch
    head mkdir perl mktemp shred egrep sed less date make env bash realpath)
    devlist=(makepkg dot)
    pkglist=(xcb-util xcb-util-wm xcb-util-xrm xcb-util-keysyms)
    for cmd in ${cmdlist[@]};do
        i=($(\builtin type -afp $cmd))
        [[ -n $i ]] || {
            \builtin printf "%s\n" "$FUNCNAME Require: $cmd"
            return
        }
        \builtin eval "local ${cmd//-/_}=${i:-:}"
    done
    for pkg in ${pkglist[@]};do
        pacman -Qi $pkg >/dev/null 2>&1 && continue
        \builtin printf "%s\n" "$FUNCNAME Require: $pkg"
        return
    done
    for cmd in ${devlist[@]};do
        i=($(\builtin type -afp $cmd))
        [[ -n $i ]] || {
            \builtin printf "%s\n" "$FUNCNAME Optional: $cmd"
            continue
        }
        \builtin eval "local ${cmd//-/_}=${i:-:}"
    done

    local perl_version="$($perl -e 'print $^V')"
    local vendor_perl=/usr/share/perl5/vendor_perl/
    local libdir=/usr/local/lib
    local includedir=/usr/local/include/
    local bindir=/usr/local/bin/
    local tmpbindir=/var/tmp/
    \builtin source <($cat<<-SUB

2bwm.testinstall()
{
    local usage="[test/tsuitX.sh file]"
    local input=\${1:?\$usage}
    local fun='2bwm.verify'
    local usrgrp="\$USER:users"
    local perm='ug=rx,o='
    local script="$bindir/\${fun}"
    \builtin source \${input} || return
    \builtin declare -F \$fun >/dev/null ||\
    { printf "%s\n" "\$FUNCNAME: \$fun not defined."; return; }
    local tmpfile=\$($mktemp)
    $sudo $rm -f \${script}
    $cat <<-BASHFUN2SCRIPT > \${tmpfile}
#!$env $bash
\$(\builtin declare -f \${fun})
\${fun}
BASHFUN2SCRIPT
    $sudo $mv \${tmpfile} \${script}
    $sudo $chmod \${perm} \${script}
    $sudo $chown \${usrgrp} \${script}
}
2bwm.graphviz()
{
    local input=\${1:?[input gv file]}
    local name=\${input##*/}
    name=\${name%%.*}
    $dot -Tpng \${input} -o /tmp/\${name}.png
    $chmod go=r /tmp/\${name}.png
}
2bwm.run()
{
    $valgrind --leak-check=full --show-leak-kinds=all src/2bwm
}
2bwm.exclude()
{
    $cat<<-DWMEXCLUDE>.git/info/exclude
pkg/
master
origin
configure
config.log
config.status
autom4te.cache/
original/
pkg/
src/2bwm
src/hidden
src/.*
test/.*
misc/.*
.*
*.o
2bwm
*~
*.xz
DWMEXCLUDE
}
2bwm.make()
{
    local config=\${1:?[vm/hostname/2bwm.conf]}
    local name=\${config##*/}
    [[ -f "\${config}" ]] || {
        \builtin echo "invalid \${config}"
        return 1
    }
    $cp -f src/config.h src/.config.h
    $cp -f "\${config}" src/config.h
    ( \builtin \cd src/ && $make clean && $make || return 1)
    [[ -f src/.config.h ]] && $mv src/.config.h src/config.h
}
2bwm.install()
{
    local bwmdate="\$($date +"%Y-%m-%d")"
    local config=\${1:?[vm/hostname/2bwm.conf]}
    2bwm.make \$config
    [[ \$? == 0 ]] || return
    $cp src/2bwm.1 src/2bwm.man
    $sed -i "s;2BWMDATE;\${bwmdate};" src/2bwm.1
    $sudo $cp -f src/2bwm.1 /usr/local/man/man1/2bwm.1
    $sudo $chmod a=r /usr/local/man/man1/2bwm.1
    $mv src/2bwm.man src/2bwm.1
    $sudo $cp -f src/2bwm ${bindir}/2bwm
    $sudo $chmod a=rx ${bindir}/2bwm
}
2bwm.build()
{
    local bwmdate="\$($date +"%Y-%m-%d")"
    local config=\${1:?[vm/hostname/2bwm.conf]}
    $chmod u=rw src/path.h
    $sed -i "s;debugging 1;debugging 0;" src/path.h
    $cp src/2bwm.1 src/2bwm.man
    2bwm.make \${config} || return 1
    $makepkg --syncdeps --force --noextract --noprepare
    $mv -f src/2bwm.man src/2bwm.1
    $sed -i "s;debugging 0;debugging 1;" src/path.h
    [[ -d pkg ]] && $mv -f 2bwm-*.tar.zst pkg/
}
2bwm.clear()
{
    $rm -rf pkg/
    $rm -f *.xz
    $rm -f *.zst
    $rm -f 2bwm
    $rm -f *.o
    $rm -f hidden
    $rm -f src/2bwm.conf
    ( \builtin \cd src/ && make clean )
}
SUB
)
}
2bwm.substitute
builtin unset -f 2bwm.substitute
