#! /bin/sh
# @file
# @brief 编译xwmo的shell辅助函数
# @author
# + 隐星魂 (Roy.Sun) <https://xwos.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0 (the "MPL"). If a copy of the MPL was not distributed
# > with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
# >
# > Alternatively, the contents of this file may be used under the
# > terms of the GNU General Public License version 2 (the "GPL"), in
# > which case the provisions of the GPL are applicable instead of the
# > above. If you wish to allow the use of your version of this file
# > only under the terms of the GPL and not to allow others to use your
# > version of this file under the MPL, indicate your decision by
# > deleting the provisions above and replace them with the notice and
# > other provisions required by the GPL. If you do not delete the
# > provisions above, a recipient may use your version of this file
# > under either the MPL or the GPL.
#

function xwmc()
{
  local cpath=$(pwd)
  local rpath=
  local prefix=
  if [[ ${cpath} =~ ${XWOS_MD_DIR} ]] ; then
    rpath=${cpath#*${XWOS_MD_DIR}}
    prefix=XWMDCFG
  elif [[ ${cpath} =~ ${XWOS_PP_DIR} ]] ; then
    rpath=${cpath#*${XWOS_PP_DIR}}
    prefix=PPCFG
  elif [[ ${cpath} =~ ${XWOS_BM_DIR} ]] ; then
    rpath=${cpath#*${XWOS_BM_DIR}}
    prefix=BMCFG
  elif [[ ${cpath} =~ ${XWOS_EM_DIR} ]] ; then
    rpath=${cpath#*${XWOS_EM_DIR}}
    prefix=XWEMCFG
  fi
  local cfg=${rpath//_/__}
  local cfg=${cfg//./_}
  local cfg=${cfg//-/_}
  local cfg=${prefix}${cfg//\//_}
  echo ${cfg}
}

function xwm()
{
  local argv=$(getopt -o B -n "${0}" -- "$@")
  eval set -- "${argv}"
  local opt_b=false
  local opt_target=
  while true ; do
    case "$1" in
      -B)
        opt_b=true
        shift 1
        ;;
      --)
        opt_target=$2
        break
        ;;
    esac
  done

  if [[ ${opt_b} = true ]] ; then
    make -C ${XWOS_BRD_DIR} clean
  fi
  make -C ${XWOS_BRD_DIR} ${opt_target}
}

function xwcroot()
{
  cd ${XWOS_PATH}
}

function xwcbd()
{
  cd ${XWOS_PATH}/${XWOS_BRD_DIR}
}
