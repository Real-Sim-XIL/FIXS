##############################################################################
### File                  : RS_DS_CM11_noSimulinkRS_AdditionalFiles.mk
### Compatibility         : DSMAKE 6.12
### Description           : Makefile fragment for additional files.
###
### Copyright 2019, dSPACE GmbH. All rights reserved.
###
##############################################################################

%  set INTERFACE_INCLUDE_MAKEFILE_VERSION := 2
% if '$(INTERFACE_IN_DS_PLATFORM_TYPE)' == 'SCALEXIO'
%     set INTERFACE_LIB_FILEPATHS +=  "C:\IPG\carmaker\win64-11.0.1\lib-dsrt\libcar4sl.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrt\libcarmaker4sl.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrt\libipgdriver.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrt\libipgroad.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrt\libipgtire.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrt\liburiparser.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrt\libz-dsrt.a" "C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b\libmatsupp-dsrt.a"
% endif
% if '$(INTERFACE_IN_DS_PLATFORM_TYPE)' == 'SCALEXIO_LNX'
%     set INTERFACE_LIB_FILEPATHS +=  "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libcar4sl.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libcarmaker4sl.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libipgdriver.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libipgroad.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libipgtire.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\liburiparser.a" "C:\IPG\carmaker\win64-11.0.1\lib-dsrtlx\libz-dsrtlx.a" "C:\IPG\carmaker\win64-11.0.1\Matlab\R2021b\libmatsupp-dsrtlx.a"
% endif
