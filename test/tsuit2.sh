2bwm.substitute()
{
    local reslist devlist libdir includedir bindir cmd i perl_version \
    tmpbindir vendor_perl \
    cmdlist='dirname basename cat ls mv sudo cp chmod ln chown rm touch
    head mkdir perl mktemp shred egrep sed less date env bash'

    declare -A Devlist=(
        [xdotool]=xdotool
        [sleep]=sleep
        [gtk-demo]=gtk-demo
        [makepkg]=makepkg
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

2bwm.fun2script()
{
    local usage="[fun][opt user:group][opt u=X,g=Y,o=Z]"
    local fun=\${1:?\$usage}
    local usrgrp=\${2:-"\$USER:users"}
    local perm=\${3:-'ug=rx,o='}
    local arg='\$@'
    local script="$bindir/\${fun}"
    \builtin declare -F \$fun >/dev/null ||\
    { printf "%s\n" "\$FUNCNAME: \$fun not defined."; return; }
    local tmpfile=\$($mktemp)
    $sudo $rm -f \${script}
    $cat <<-BASHFUN2SCRIPT > \${tmpfile}
#!$env $bash
\$(\builtin declare -f \${fun})
\${fun} \${arg}
BASHFUN2SCRIPT
    $sudo $mv \${tmpfile} \${script}
    $sudo $chmod \${perm} \${script}
    $sudo $chown \${usrgrp} \${script}
}
2bwm.verify()
{
    # These test cases should no be executed from a terminal.
    # They will occupy workspace 1 and 2.
    declare -a Tests=(
    2bwm.verify1   
    )
    2bwm.verify1()
    {
        # 
        $sleep 1
        $xdotool key super+1
        $sleep 1
        $xdotool key super+ctrl+8
        $sleep 1
        $xdotool key super+p
        $sleep 1
    }
    local fun
    set -o xtrace
    for fun in \${Tests[@]};do
    $cat <<-2BWMVERIFY > ${tmpbindir}/\${fun}
#!$env $bash
\$(\builtin declare -f \${fun})
\${fun} '\$@'
2BWMVERIFY
    \builtin unset -f \${fun}
    $chmod u=rwx ${tmpbindir}/\${fun}
    ${tmpbindir}/\${fun}
    $rm -f ${tmpbindir}/\${fun}
    done
    set +o xtrace
}
SUB
)
}
2bwm.substitute
builtin unset -f 2bwm.substitute
