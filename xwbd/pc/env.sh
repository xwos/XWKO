#! /bin/bash
# @file
# @brief environment-setup shell script
# @author
# + 隐星魂 (Roy.Sun) <www.starsoul.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

if [[ ${BASH_SOURCE[0]} = ${0} ]] ;then
	echo -e "\033[33;1mThis script must be sourced by bash!\033[0m"
	exit 0
fi

cd $(dirname ${BASH_SOURCE[0]})
make cfg
source wkspc/env.rc
xwcroot
