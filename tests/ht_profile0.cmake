# Decoding
add_test(NAME dec_p0_ht_01_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_01_b11.j2k -o ht_p0_01_b11.pgx)
add_test(NAME dec_p0_ht_02_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_02_b11.j2k -o ht_p0_02_b11.pgx)
add_test(NAME dec_p0_ht_03_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_03_b11.j2k -o ht_p0_03_b11.pgx)
add_test(NAME dec_p0_ht_03_14 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_03_b14.j2k -o ht_p0_03_b14.pgx)
add_test(NAME dec_p0_ht_04_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_04_b11.j2k -o ht_p0_04_b11.pgx)
add_test(NAME dec_p0_ht_04_12 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_04_b12.j2k -o ht_p0_04_b12.pgx)
add_test(NAME dec_p0_ht_05_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_05_b11.j2k -o ht_p0_05_b11.pgx)
add_test(NAME dec_p0_ht_05_12 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_05_b12.j2k -o ht_p0_05_b12.pgx)
add_test(NAME dec_p0_ht_06_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_06_b11.j2k -o ht_p0_06_b11.pgx)
add_test(NAME dec_p0_ht_06_15 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_06_b15.j2k -o ht_p0_06_b15.pgx)
add_test(NAME dec_p0_ht_06_18 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_06_b18.j2k -o ht_p0_06_b18.pgx)
add_test(NAME dec_p0_hm_06_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_hm_06_b11.j2k -o hm_p0_06_b11.pgx)
add_test(NAME dec_p0_hm_06_18 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_hm_06_b18.j2k -o hm_p0_06_b18.pgx)
add_test(NAME dec_p0_ht_07_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_07_b11.j2k -o ht_p0_07_b11.pgx)
add_test(NAME dec_p0_ht_07_15 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_07_b15.j2k -o ht_p0_07_b15.pgx)
add_test(NAME dec_p0_ht_07_16 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_07_b16.j2k -o ht_p0_07_b16.pgx)
add_test(NAME dec_p0_ht_08_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_08_b11.j2k -o ht_p0_08_b11.pgx -reduce 1)
add_test(NAME dec_p0_ht_08_15 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_08_b15.j2k -o ht_p0_08_b15.pgx -reduce 1)
add_test(NAME dec_p0_ht_08_16 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_08_b16.j2k -o ht_p0_08_b16.pgx -reduce 1)
add_test(NAME dec_p0_ht_09_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_09_b11.j2k -o ht_p0_09_b11.pgx)
add_test(NAME dec_p0_ht_10_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_10_b11.j2k -o ht_p0_10_b11.pgx)
add_test(NAME dec_p0_ht_11_10 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_11_b10.j2k -o ht_p0_11_b10.pgx)
add_test(NAME dec_p0_ht_12_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_12_b11.j2k -o ht_p0_12_b11.pgx)
add_test(NAME dec_p0_ht_14_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_14_b11.j2k -o ht_p0_14_b11.pgx)
add_test(NAME dec_p0_ht_15_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_15_b11.j2k -o ht_p0_15_b11.pgx)
add_test(NAME dec_p0_ht_15_14 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_15_b14.j2k -o ht_p0_15_b14.pgx)
add_test(NAME dec_p0_hm_15_8 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_hm_15_b8.j2k -o hm_p0_15_b8.pgx)
add_test(NAME dec_p0_ht_16_11 COMMAND open_htj2k_dec -i ${CONFORMANCE_DATA_DIR}/ds0_ht_16_b11.j2k -o ht_p0_16_b11.pgx)

# calculate PAE and MSE
# 1
add_test(NAME comp_p0_ht_01_11 COMMAND imgcmp ht_p0_01_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_01-0.pgx 0 0)
set_tests_properties(comp_p0_ht_01_11 PROPERTIES DEPENDS dec_p0_ht_01_11)
# 2
add_test(NAME comp_p0_ht_02_11 COMMAND imgcmp ht_p0_02_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_02-0.pgx 1 0.001)
set_tests_properties(comp_p0_ht_02_11 PROPERTIES DEPENDS dec_p0_ht_02_11)
# 3
add_test(NAME comp_p0_ht_03_11 COMMAND imgcmp ht_p0_03_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_03-0.pgx 17 0.15)
add_test(NAME comp_p0_ht_03_14 COMMAND imgcmp ht_p0_03_b14_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_03-0.pgx 0 0)
set_tests_properties(comp_p0_ht_03_11 PROPERTIES DEPENDS dec_p0_ht_03_11)
set_tests_properties(comp_p0_ht_03_14 PROPERTIES DEPENDS dec_p0_ht_03_14)
# 4
add_test(NAME comp_p0_ht_04_11r COMMAND imgcmp ht_p0_04_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_04-0.pgx 7 0.876)
add_test(NAME comp_p0_ht_04_11g COMMAND imgcmp ht_p0_04_b11_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_04-1.pgx 6 0.726)
add_test(NAME comp_p0_ht_04_11b COMMAND imgcmp ht_p0_04_b11_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_04-2.pgx 8 1.170)
add_test(NAME comp_p0_ht_04_12r COMMAND imgcmp ht_p0_04_b12_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_04-0.pgx 5 0.776)
add_test(NAME comp_p0_ht_04_12g COMMAND imgcmp ht_p0_04_b12_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_04-1.pgx 4 0.626)
add_test(NAME comp_p0_ht_04_12b COMMAND imgcmp ht_p0_04_b12_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_04-2.pgx 6 1.070)
set_tests_properties(comp_p0_ht_04_11r comp_p0_ht_04_11g comp_p0_ht_04_11b PROPERTIES DEPENDS dec_p0_ht_04_11)
set_tests_properties(comp_p0_ht_04_12r comp_p0_ht_04_12g comp_p0_ht_04_12b PROPERTIES DEPENDS dec_p0_ht_04_12)
# 5
add_test(NAME comp_p0_ht_05_11a COMMAND imgcmp ht_p0_05_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_05-0.pgx 2 0.319)
add_test(NAME comp_p0_ht_05_11b COMMAND imgcmp ht_p0_05_b11_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_05-1.pgx 3 0.324) # plus 0.001
add_test(NAME comp_p0_ht_05_11c COMMAND imgcmp ht_p0_05_b11_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_05-2.pgx 3 0.318) # plus 0.001
add_test(NAME comp_p0_ht_05_11d COMMAND imgcmp ht_p0_05_b11_03.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_05-3.pgx 2 0.001) # plus 0.001
add_test(NAME comp_p0_ht_05_12a COMMAND imgcmp ht_p0_05_b12_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_05-0.pgx 2 0.319) # 0.302 in old spec
add_test(NAME comp_p0_ht_05_12b COMMAND imgcmp ht_p0_05_b12_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_05-1.pgx 2 0.323) # 0.307 in old spec
add_test(NAME comp_p0_ht_05_12c COMMAND imgcmp ht_p0_05_b12_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_05-2.pgx 2 0.317) # 0.269 in old spec
add_test(NAME comp_p0_ht_05_12d COMMAND imgcmp ht_p0_05_b12_03.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_05-3.pgx 0 0)
set_tests_properties(comp_p0_ht_05_11a comp_p0_ht_05_11b comp_p0_ht_05_11c comp_p0_ht_05_11d PROPERTIES DEPENDS dec_p0_ht_05_11)
set_tests_properties(comp_p0_ht_05_12a comp_p0_ht_05_12b comp_p0_ht_05_12c comp_p0_ht_05_12d PROPERTIES DEPENDS dec_p0_ht_05_12)
# 6
add_test(NAME comp_p0_ht_06_11a COMMAND imgcmp ht_p0_06_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-0.pgx 3135 86287)
add_test(NAME comp_p0_ht_06_11b COMMAND imgcmp ht_p0_06_b11_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-1.pgx 403 6124)
add_test(NAME comp_p0_ht_06_11c COMMAND imgcmp ht_p0_06_b11_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-2.pgx 378 3968)
add_test(NAME comp_p0_ht_06_11d COMMAND imgcmp ht_p0_06_b11_03.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-3.pgx 200 2000)
add_test(NAME comp_p0_ht_06_15a COMMAND imgcmp ht_p0_06_b15_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-0.pgx 635 11287)
add_test(NAME comp_p0_ht_06_15b COMMAND imgcmp ht_p0_06_b15_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-1.pgx 403 6124)
add_test(NAME comp_p0_ht_06_15c COMMAND imgcmp ht_p0_06_b15_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-2.pgx 378 3968)
add_test(NAME comp_p0_ht_06_15d COMMAND imgcmp ht_p0_06_b15_03.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-3.pgx 12 10.0)
add_test(NAME comp_p0_ht_06_18a COMMAND imgcmp ht_p0_06_b18_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-0.pgx 635 11287)
add_test(NAME comp_p0_ht_06_18b COMMAND imgcmp ht_p0_06_b18_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-1.pgx 403 6124)
add_test(NAME comp_p0_ht_06_18c COMMAND imgcmp ht_p0_06_b18_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-2.pgx 378 3968)
add_test(NAME comp_p0_ht_06_18d COMMAND imgcmp ht_p0_06_b18_03.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-3.pgx 0 0)
add_test(NAME comp_p0_hm_06_11a COMMAND imgcmp hm_p0_06_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-0.pgx 635 11287)
add_test(NAME comp_p0_hm_06_11b COMMAND imgcmp hm_p0_06_b11_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-1.pgx 403 6124)
add_test(NAME comp_p0_hm_06_11c COMMAND imgcmp hm_p0_06_b11_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-2.pgx 378 3968)
add_test(NAME comp_p0_hm_06_11d COMMAND imgcmp hm_p0_06_b11_03.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-3.pgx 200 2000)
add_test(NAME comp_p0_hm_06_18a COMMAND imgcmp hm_p0_06_b18_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-0.pgx 635 11287)
add_test(NAME comp_p0_hm_06_18b COMMAND imgcmp hm_p0_06_b18_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-1.pgx 403 6124)
add_test(NAME comp_p0_hm_06_18c COMMAND imgcmp hm_p0_06_b18_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-2.pgx 378 3968)
add_test(NAME comp_p0_hm_06_18d COMMAND imgcmp hm_p0_06_b18_03.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_06-3.pgx 0 0)
set_tests_properties(comp_p0_ht_06_11a comp_p0_ht_06_11b comp_p0_ht_06_11c comp_p0_ht_06_11d PROPERTIES DEPENDS dec_p0_ht_06_11)
set_tests_properties(comp_p0_ht_06_15a comp_p0_ht_06_15b comp_p0_ht_06_15c comp_p0_ht_06_15d PROPERTIES DEPENDS dec_p0_ht_06_15)
set_tests_properties(comp_p0_ht_06_18a comp_p0_ht_06_18b comp_p0_ht_06_18c comp_p0_ht_06_18d PROPERTIES DEPENDS dec_p0_ht_06_18)
set_tests_properties(comp_p0_hm_06_11a comp_p0_hm_06_11b comp_p0_hm_06_11c comp_p0_hm_06_11d PROPERTIES DEPENDS dec_p0_hm_06_11)
set_tests_properties(comp_p0_hm_06_18a comp_p0_hm_06_18b comp_p0_hm_06_18c comp_p0_hm_06_18d PROPERTIES DEPENDS dec_p0_hm_06_18)
# 7
add_test(NAME comp_p0_ht_07_11r COMMAND imgcmp ht_p0_07_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-0.pgx 40 25.0)
add_test(NAME comp_p0_ht_07_11g COMMAND imgcmp ht_p0_07_b11_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-1.pgx 40 25.0)
add_test(NAME comp_p0_ht_07_11b COMMAND imgcmp ht_p0_07_b11_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-2.pgx 50 25.0)
add_test(NAME comp_p0_ht_07_15r COMMAND imgcmp ht_p0_07_b15_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-0.pgx 2 0.075)
add_test(NAME comp_p0_ht_07_15g COMMAND imgcmp ht_p0_07_b15_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-1.pgx 2 0.05)
add_test(NAME comp_p0_ht_07_15b COMMAND imgcmp ht_p0_07_b15_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-2.pgx 2 0.075)
add_test(NAME comp_p0_ht_07_16r COMMAND imgcmp ht_p0_07_b16_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-0.pgx 0 0)
add_test(NAME comp_p0_ht_07_16g COMMAND imgcmp ht_p0_07_b16_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-1.pgx 0 0)
add_test(NAME comp_p0_ht_07_16b COMMAND imgcmp ht_p0_07_b16_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_07-2.pgx 0 0)
set_tests_properties(comp_p0_ht_07_11r comp_p0_ht_07_11g comp_p0_ht_07_11b PROPERTIES DEPENDS dec_p0_ht_07_11)
set_tests_properties(comp_p0_ht_07_15r comp_p0_ht_07_15g comp_p0_ht_07_15b PROPERTIES DEPENDS dec_p0_ht_07_15)
set_tests_properties(comp_p0_ht_07_16r comp_p0_ht_07_16g comp_p0_ht_07_16b PROPERTIES DEPENDS dec_p0_ht_07_16)
# 8
add_test(NAME comp_p0_ht_08_11r COMMAND imgcmp ht_p0_08_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-0.pgx 40 45.0)
add_test(NAME comp_p0_ht_08_11g COMMAND imgcmp ht_p0_08_b11_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-1.pgx 40 30.0)
add_test(NAME comp_p0_ht_08_11b COMMAND imgcmp ht_p0_08_b11_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-2.pgx 40 45.0)
add_test(NAME comp_p0_ht_08_15r COMMAND imgcmp ht_p0_08_b15_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-0.pgx 0 0)
add_test(NAME comp_p0_ht_08_15g COMMAND imgcmp ht_p0_08_b15_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-1.pgx 1 0.001)
add_test(NAME comp_p0_ht_08_15b COMMAND imgcmp ht_p0_08_b15_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-2.pgx 0 0)
add_test(NAME comp_p0_ht_08_16r COMMAND imgcmp ht_p0_08_b16_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-0.pgx 0 0)
add_test(NAME comp_p0_ht_08_16g COMMAND imgcmp ht_p0_08_b16_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-1.pgx 0 0)
add_test(NAME comp_p0_ht_08_16b COMMAND imgcmp ht_p0_08_b16_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_08-2.pgx 0 0)
set_tests_properties(comp_p0_ht_08_11r comp_p0_ht_08_11g comp_p0_ht_08_11b PROPERTIES DEPENDS dec_p0_ht_08_11)
set_tests_properties(comp_p0_ht_08_15r comp_p0_ht_08_15g comp_p0_ht_08_15b PROPERTIES DEPENDS dec_p0_ht_08_15)
set_tests_properties(comp_p0_ht_08_16r comp_p0_ht_08_16g comp_p0_ht_08_16b PROPERTIES DEPENDS dec_p0_ht_08_16)
# 9
add_test(NAME comp_p0_ht_09_11 COMMAND imgcmp ht_p0_09_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_09-0.pgx 0 0)
set_tests_properties(comp_p0_ht_09_11 PROPERTIES DEPENDS dec_p0_ht_09_11)
# 10
add_test(NAME comp_p0_ht_10_11r COMMAND imgcmp ht_p0_10_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_10-0.pgx 0 0)
add_test(NAME comp_p0_ht_10_11g COMMAND imgcmp ht_p0_10_b11_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_10-1.pgx 0 0)
add_test(NAME comp_p0_ht_10_11b COMMAND imgcmp ht_p0_10_b11_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_10-2.pgx 0 0)
set_tests_properties(comp_p0_ht_10_11r comp_p0_ht_10_11g comp_p0_ht_10_11b PROPERTIES DEPENDS dec_p0_ht_10_11)
# 11
add_test(NAME comp_p0_ht_11_10 COMMAND imgcmp ht_p0_11_b10_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_11-0.pgx 0 0)
set_tests_properties(comp_p0_ht_11_10 PROPERTIES DEPENDS dec_p0_ht_11_10)
# 12
add_test(NAME comp_p0_ht_12_11 COMMAND imgcmp ht_p0_12_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_12-0.pgx 0 0)
set_tests_properties(comp_p0_ht_12_11 PROPERTIES DEPENDS dec_p0_ht_12_11)
# 14
add_test(NAME comp_p0_ht_14_11r COMMAND imgcmp ht_p0_14_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_14-0.pgx 0 0)
add_test(NAME comp_p0_ht_14_11g COMMAND imgcmp ht_p0_14_b11_01.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_14-1.pgx 0 0)
add_test(NAME comp_p0_ht_14_11b COMMAND imgcmp ht_p0_14_b11_02.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_14-2.pgx 0 0)
set_tests_properties(comp_p0_ht_14_11r comp_p0_ht_14_11g comp_p0_ht_14_11b PROPERTIES DEPENDS dec_p0_ht_14_11)
# 15
add_test(NAME comp_p0_ht_15_11 COMMAND imgcmp ht_p0_15_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_15-0.pgx 17 0.15)
add_test(NAME comp_p0_ht_15_14 COMMAND imgcmp ht_p0_15_b14_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_15-0.pgx 0 0)
add_test(NAME comp_p0_hm_15_8 COMMAND imgcmp hm_p0_15_b8_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_15-0.pgx 0 0)
set_tests_properties(comp_p0_ht_15_11 PROPERTIES DEPENDS dec_p0_ht_15_11)
set_tests_properties(comp_p0_ht_15_14 PROPERTIES DEPENDS dec_p0_ht_15_14)
set_tests_properties(comp_p0_hm_15_8 PROPERTIES DEPENDS dec_p0_hm_15_8)
# 16
add_test(NAME comp_p0_ht_16_11 COMMAND imgcmp ht_p0_16_b11_00.pgx ${CONFORMANCE_DATA_DIR}/references/c1p0_16-0.pgx 0 0)
set_tests_properties(comp_p0_ht_16_11 PROPERTIES DEPENDS dec_p0_ht_16_11)
