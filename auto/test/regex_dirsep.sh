#!/bin/sh

ngx_regex_dirsep="\/"
ngx_dirsep='/'

NGX_OBJS=objs
ngx_objs_dir=${NGX_OBJS}${ngx_regex_dirsep}
echo "ngx_objs_dir is $ngx_objs_dir"

echo "ngx_regex_dirsep is '$ngx_regex_dirsep'"
echo "ngx_dirsep is '$ngx_dirsep'"