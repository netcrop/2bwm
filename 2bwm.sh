2bwm.substitute()
{
    local reslist devlist libdir includedir bindir cmd i perl_version \
    tmpbindir vendor_perl \
    cmdlist='dirname basename cat ls mv sudo cp chmod ln chown rm touch
    head mkdir perl mktemp shred egrep sed less date make env bash realpath'

    declare -A Devlist=(
        [makepkg]=makepkg
        [dot]=dot
    )
    cmdlist="${Devlist[@]} $cmdlist"
    for cmd in $cmdlist;do
        i=($(\builtin type -afp $cmd 2>/dev/null))
        if [[ -z $i ]];then
            if [[ -z ${Devlist[$cmd]} ]];then
                reslist+=" $cmd"
            else
                devlist+=" $cmd"
            fi
        fi
        \builtin eval "local ${cmd//-/_}=${i:-:}"
    done
    [[ -z $reslist ]] ||\
    { 
        \builtin printf "%s\n" \
        "$FUNCNAME says: ( $reslist ) These Required Commands are missing."
        return
    }
    [[ -z $devlist ]] ||\
    \builtin printf "%s\n" \
    "$FUNCNAME says: ( $devlist ) These Optional Commands for further development."

    perl_version="$($perl -e 'print $^V')"
    vendor_perl=/usr/share/perl5/vendor_perl/
    libdir=/usr/local/lib
    includedir=/usr/local/include/
    bindir=/usr/local/bin/
    tmpbindir=/var/tmp/
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
pkg
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
    [[ -f "\$($realpath config.h)" ]] && {
        $cp -f src/config.h src/.config.h
        $cp -f config.h src/
    }
    ( \builtin \cd src/ && $make clean && $make )
    [[ -f src/.config.h ]] && $mv src/.config.h src/config.h
}
2bwm.install()
{
    local bwmdate="\$($date +"%Y-%m-%d")"
    2bwm.make
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
    $chmod u=rw src/path.h
    $sed -i "s;debugging 1;debugging 0;" src/path.h
    $cp src/2bwm.1 src/2bwm.man
    $sed -i "s;2BWMDATE;\${bwmdate};" src/2bwm.1
    2bwm.make
    $makepkg --syncdeps --force --noextract --noprepare
    $sed -i "s;debugging 0;debugging 1;" src/path.h
    $mv src/2bwm.man src/2bwm.1
}
2bwm.clear()
{
    $rm -rf pkg/
    $rm -f *.xz
    $rm -f *.zst
    $rm -f 2bwm
    $rm -f *.o
    $rm -f hidden
    ( \builtin \cd src/ && make clean )
}
SUB
)
}
2bwm.substitute
builtin unset -f 2bwm.substitute
