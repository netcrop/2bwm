2bwm.substitute()
{
    local reslist devlist libdir includedir bindir cmd i perl_version \
    tmpbindir outputdir vendor_perl \
    cmdlist='dirname basename cat ls mv sudo cp chmod ln chown rm touch
    head mkdir perl mktemp shred egrep sed less date env bash xdotool sleep
    gtk-demo figlet seq magick import urxvt'

    declare -A Devlist=(
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
    outputdir=/var/tmp/2bwm/
    workspace=7
    mkdir -p ${outputdir}
    \builtin source <($cat<<-SUB

2bwm.verify()
{
    # These test cases should no be executed from a terminal.
    # They will occupy workspace 1 and 2.
    declare -a Tests=(
        2bwm.verify1
    )
    2bwm.printscreen()
    {
        local index=\${1:?[output file index]}
        local output=${outputdir}/\${index}.jpg
        $import -crop "800x600+1520+780" -window root -screen \${output}  
    }
    2bwm.terminal()
    {
        $urxvt -geometry '20x15+1760+840'
    }
    2bwm.verify1()
    {
        local i timer='0.3' total=6
        $xdotool sleep 1 key super+${workspace} index=0
        for i in \$($seq \${total});do
            $xdotool sleep \${timer} key super+Return
            $xdotool sleep \${timer} key super+ctrl+8
            $xdotool sleep \${timer} type "PS1=''"
            $xdotool key --clearmodifiers --delay 0 Return Ctrl+l 
           $xdotool sleep \${timer} type --clearmodifiers "$figlet -f block \$'\n'\${i}"
            $xdotool key --delay 0 Return
        done
        $xdotool sleep \${timer} key super+s
        $xdotool sleep \${timer} key super+j
        $xdotool sleep \${timer} key super+ctrl+Tab   
        $xdotool sleep \${timer} key super+j super+j
        $xdotool sleep \${timer} key super+Tab
        $xdotool sleep \${timer} key super+j
        $xdotool sleep \${timer} key super+ctrl+Tab   
        $xdotool sleep \${timer} key super+j super+j

        $xdotool sleep \${timer} key super+s
        $xdotool sleep \${timer} key super+s
        $xdotool sleep \${timer} key super+Tab
        $xdotool sleep \${timer} key super+s
        $xdotool sleep \${timer} key super+s

        for i in \$($seq \${total});do
            $xdotool sleep \${timer} key super+p
        done
    }
    set +o xtrace
    \${Tests[@]}
    \builtin unset -f \${Tests[@]}
    set +o xtrace
}
SUB
)
}
2bwm.substitute
builtin unset -f 2bwm.substitute