#!/bin/bash


perl -w TI_MIP_expl.pl ~/instance_CECSP/instance_CECSP_identite/OR_spectrum_sans_function/ ~/Dev2016/CECSP/resultats_2017/CPAIOR/sol_TI/res_TIid;
perl -w TI_MIP_expl.pl ~/instance_CECSP/instance_CECSP_function/OR_spectrum_function/avec_function/ ~/Dev2016/CECSP/resultats_2017/CPAIOR/sol_TI/res_TIv1;
perl -w TI_MIP_expl.pl ~/instance_CECSP/instance_CECSP_function/OR_spectrum_function/avec_function_v2/ ~/Dev2016/CECSP/resultats_2017/CPAIOR/sol_TI/res_TIv2;
perl -w TI_MIP_expl.pl ~/instance_CECSP/instance_CECSP_function/OR_spectrum_function/avec_function_v3/todo/ ~/Dev2016/CECSP/resultats_2017/CPAIOR/sol_TI/res_TIv3
