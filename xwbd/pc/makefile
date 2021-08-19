#! /bin/make -f
# @file
# @brief Top makefile of board/project
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

XuanWuKO ?= $(shell cd ../.. && pwd)
WKSPC ?= wkspc

MKCFG := $(XuanWuKO)/xwbs/util/el/mkcfg.el
WKSPC := $(shell [ -d $(WKSPC) ] || mkdir -p $(WKSPC) && cd $(WKSPC) && pwd)

modules: cfg
	$(MAKE) -C $(XuanWuKO) M=$(WKSPC) $@

modules_install: modules
	$(MAKE) -C $(XuanWuKO) M=$(WKSPC) $@

cfg: $(WKSPC)/XuanWuOS.cfg

$(WKSPC)/XuanWuOS.cfg: cfg/*.h
	@[ -x $(MKCFG) ] || chmod +x $(MKCFG)
	@${MKCFG} --XuanWuOS=$(XuanWuKO) --cfgdir=cfg --wkspc=$(WKSPC) -- $(@F)
	@ln -snf $(XuanWuKO)/Makefile $(WKSPC)/Makefile

c clean: cfg
	$(MAKE) -C $(XuanWuKO) M=$(WKSPC) clean

d distclean:
	$(RM) -rf $(WKSPC)

.DEFAULT: cfg
	$(MAKE) -C $(XuanWuKO) M=$(WKSPC) $@

.PHONY : modules modules_install cfg c clean
