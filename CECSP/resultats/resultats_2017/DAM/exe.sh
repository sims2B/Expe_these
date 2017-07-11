#/bin/bash/

echo "nocut \n";
./a.out res_OOv3_nocut_obj_TL1000_0_0_0_1_1_0_0_0_0.txt best_sol_v3 best_LB_v3 ;
echo 'def';
./a.out res_OOv3_obj_TL1000_0_0_0_1_1_0_0_0_0.txt best_sol_v3 best_LB_v3 ;
echo 'int';
./a.out res_OOv3_obj_TL1000_0_1_1_0_1_0_0_0_0.txt best_sol_v3 best_LB_v3 ;
echo 'time';
./a.out res_OOv3_obj_TL1000_0_0_0_0_1_1_1_0_0.txt best_sol_v3 best_LB_v3 ;
echo 'KS';
./a.out res_OOv3_obj_TL1000_0_0_0_1_1_0_0_1_0.txt best_sol_v3 best_LB_v3 ;
echo 'Preemp';
./a.out res_OOv3_obj_TL1000_1_0_0_1_1_0_0_0_0.txt best_sol_v3 best_LB_v3 ;
echo 'ITK';
./a.out res_OOv3_obj_TL1000_0_1_1_0_1_1_1_1_0.txt best_sol_v3 best_LB_v3 ;
echo 'ITP';
./a.out res_OOv3_obj_TL1000_1_1_1_0_1_1_1_0_0.txt best_sol_v3 best_LB_v3 ;
echo 'IKP';
./a.out res_OOv3_obj_TL1000_1_1_1_0_1_0_0_1_0.txt best_sol_v3 best_LB_v3 ;
echo 'TKP';
./a.out res_OOv3_obj_TL1000_1_0_0_1_1_1_1_1_0.txt best_sol_v3 best_LB_v3 ;
echo 'ITKP';
./a.out res_OOv3_obj_TL1000_1_1_1_0_1_1_1_1_0.txt best_sol_v3 best_LB_v3 ;
