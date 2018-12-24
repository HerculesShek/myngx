#!/bin/sh
if echo "test\c" | grep c >/dev/null; then

    if echo -n test | grep n >/dev/null; then
        echo "1"
        ngx_n=
        ngx_c=

    else # linux here
        echo "2"
        ngx_n=-n
        ngx_c=
    fi

else # mac here
    echo "3"
    ngx_n=
    ngx_c='\c'
fi