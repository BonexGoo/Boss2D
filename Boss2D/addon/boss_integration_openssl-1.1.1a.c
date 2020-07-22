#include <boss.h>
#if BOSS_NEED_ADDON_CURL | BOSS_NEED_ADDON_GIT | BOSS_NEED_ADDON_SSL | BOSS_NEED_ADDON_WEBRTC | BOSS_NEED_ADDON_WEBSOCKET

#include <addon/boss_integration_openssl-1.1.1a.h>

#if BOSS_WINDOWS
    #pragma comment(lib, "advapi32.lib")
    #pragma comment(lib, "gdi32.lib")
#endif

#pragma warning(disable : 4090)
#pragma warning(disable : 4133)

#if !BOSS_WINDOWS | BOSS_NEED_FORCED_FAKEWIN
    #define __pragma(...)
    #define struct_stat_BOSS struct boss_fakewin_struct_stat64
    void OutputDebugString(const TCHAR* buf)
    {
        BOSS_TRACE("%s", buf);
    }
#else
    #define struct_stat_BOSS struct stat
#endif

NON_EMPTY_TRANSLATION_UNIT

//#include <addon/openssl-1.1.1a_for_boss/crypto/armcap.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/cpt_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/cryptlib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ctype.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/cversion.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/dllmain.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ebcdic.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ex_data.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/getenv.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/init.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/LPdir_nyi.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/LPdir_unix.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/LPdir_vms.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/LPdir_win.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/LPdir_win32.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/LPdir_wince.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/mem.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/mem_clr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/mem_dbg.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/mem_sec.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/o_dir.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/o_fips.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/o_fopen.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/o_init.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/o_str.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/o_time.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/ppccap.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/s390xcap.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/sparcv9cap.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/threads_none.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/threads_pthread.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/threads_win.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/uid.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/objects/o_names.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/objects/obj_dat.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/objects/obj_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/objects/obj_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/objects/obj_xref.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/err/err.c>
#define print_bio print_bio_err_prn_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/err/err_prn.c>
#undef print_bio
#include <addon/openssl-1.1.1a_for_boss/crypto/err/err_all.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/bio/b_addr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/b_dump.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/b_print.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/b_sock.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/b_sock2.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bf_buff.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bf_lbuf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bf_nbio.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bf_null.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bio_cb.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bio_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bio_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bio_meth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_acpt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_bio.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_conn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_dgram.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_fd.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_file.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_log.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_mem.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_null.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bio/bss_sock.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_ameth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_chk.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_crpt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_depr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_gen.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_meth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_mp.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_none.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_oaep.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_ossl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_pk1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_pmeth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_prn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_pss.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_saos.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_sign.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_ssl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_x931.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rsa/rsa_x931g.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_ameth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_depr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_gen.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_key.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_meth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_ossl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_pmeth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_prn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_sign.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dsa/dsa_vrf.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/rc4/rc4_skey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rc4/rc4_enc.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/sha/keccak1600.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/sha/sha1dgst.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/sha/sha256.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/sha/sha512.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/sha/sha1_one.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/md5/md5_dgst.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/md5/md5_one.c>

#define u128 u128_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/curve25519.c>
#undef u128
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec2_oct.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec2_smpl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_ameth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_check.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_curve.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_cvt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_key.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_kmeth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_mult.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_oct.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_pmeth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ec_print.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecdh_kdf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecdh_ossl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecdsa_ossl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecdsa_sign.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecdsa_vrf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/eck_prn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_mont.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_nist.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_nistp224.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_nistp256.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_nistp521.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_nistputil.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_nistz256.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_nistz256_table.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_oct.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecp_smpl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/ecx_meth.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/curve448.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/curve448_tables.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/eddsa.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/f_generic.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/scalar.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/ec/curve448/arch_32/f_impl.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/x509/by_dir.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/by_file.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/t_crl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/t_req.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/t_x509.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_att.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_cmp.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_d2.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_def.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_ext.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_lu.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_meth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_obj.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_r2x.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_req.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_set.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_trs.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_txt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_v3.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_vfy.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509_vpm.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509cset.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509name.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509rset.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509spki.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x509type.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_all.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_attrib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_crl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_exten.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_name.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_pubkey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_req.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_x509.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509/x_x509a.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/pcy_cache.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/pcy_data.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/pcy_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/pcy_map.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/pcy_node.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/pcy_tree.c>
#define extract_min_max extract_min_max_v3_addr_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_addr.c>
#undef extract_min_max
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_admis.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_akey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_akeya.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_alt.c>
#define extract_min_max extract_min_max_v3_asid_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_asid.c>
#undef extract_min_max
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_bcons.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_bitst.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_conf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_cpols.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_crld.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_enum.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_extku.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_genn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_ia5.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_info.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_int.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_ncons.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_pci.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_pcia.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_pcons.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_pku.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_pmaps.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_prn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_purp.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_skey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_sxnet.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_tlsf.c>
#define str_free str_free_v3_utl_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3_utl.c>
#undef str_free
#include <addon/openssl-1.1.1a_for_boss/crypto/x509v3/v3err.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs7/bio_pk7.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs7/pk7_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs7/pk7_attr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs7/pk7_doit.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs7/pk7_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs7/pk7_mime.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs7/pk7_smime.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs7/pkcs7err.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_add.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_asn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_attr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_crpt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_crt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_decr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_init.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_key.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_kiss.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_mutl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_npas.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_p8d.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_p8e.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_sbag.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/p12_utl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pkcs12/pk12err.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_asn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_cl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_ext.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_ht.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_prn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_srv.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/ocsp_vfy.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ocsp/v3_ocsp.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/evp/bio_b64.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/bio_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/bio_md.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/bio_ok.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/c_allc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/c_alld.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/cmeth_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/digest.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_aes.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_aes_cbc_hmac_sha1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_aes_cbc_hmac_sha256.c>
#define ctr64_inc ctr64_inc_e_aria_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_aria.c>
#undef ctr64_inc
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_bf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_camellia.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_cast.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_chacha20_poly1305.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_des.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_des3.c>
#undef ks1
#undef ks2
#undef ks3
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_idea.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_null.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_old.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_rc2.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_rc4.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_rc4_hmac_md5.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_rc5.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_seed.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_sm4.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/e_xcbc_d.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/encode.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/evp_cnf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/evp_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/evp_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/evp_key.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/evp_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/evp_pbe.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/evp_pkey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_md2.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_md4.c>
#define init init_m_md5_BOSS
#define update update_m_md5_BOSS
#define final final_m_md5_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_md5.c>
#undef init
#undef update
#undef final
#define init init_m_md5_sha1_BOSS
#define update update_m_md5_sha1_BOSS
#define final final_m_md5_sha1_BOSS
#define ctrl ctrl_m_md5_sha1_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_md5_sha1.c>
#undef init
#undef update
#undef final
#undef ctrl
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_mdc2.c>
#define init init_m_null_BOSS
#define update update_m_null_BOSS
#define final final_m_null_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_null.c>
#undef init
#undef update
#undef final
#define init init_m_ripemd_BOSS
#define update update_m_ripemd_BOSS
#define final final_m_ripemd_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_ripemd.c>
#undef init
#undef update
#undef final
#define init init_m_sha1_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_sha1.c>
#undef init
#define init init_m_sha3_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_sha3.c>
#undef init
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_sigver.c>
#define init init_m_wp_BOSS
#define update update_m_wp_BOSS
#define final final_m_wp_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/m_wp.c>
#undef init
#undef update
#undef final
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/names.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p5_crpt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p5_crpt2.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p_dec.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p_open.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p_seal.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p_sign.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/p_verify.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/pbe_scrypt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/pmeth_fn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/pmeth_gn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/evp/pmeth_lib.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/ui/ui_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ui/ui_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ui/ui_null.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ui/ui_openssl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ui/ui_util.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_bitstr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_d2i_fp.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_digest.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_dup.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_gentm.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_i2d_fp.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_int.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_mbstr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_object.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_octet.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_print.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_sign.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_strex.c>
#define table table_a_strnid_BOSS
#define table_cmp table_a_strnid_BOSS_cmp
#define table_cmp_BSEARCH_CMP_FN table_a_strnid_BOSS_cmp_BSEARCH_CMP_FN
#define OBJ_bsearch_table OBJ_bsearch_table_a_strnid_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_strnid.c>
#undef table
#undef table_cmp
#undef table_cmp_BSEARCH_CMP_FN
#undef OBJ_bsearch_table
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_time.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_type.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_utctm.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_utf8.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/a_verify.c>
#define standard_methods standard_methods_ameth_lib_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/ameth_lib.c>
#undef standard_methods
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn1_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn1_gen.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn1_item_list.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn1_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn1_par.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn_mime.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn_moid.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn_mstbl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/asn_pack.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/bio_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/bio_ndef.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/d2i_pr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/d2i_pu.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/evp_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/f_int.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/f_string.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/i2d_pr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/i2d_pu.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/n_pkey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/nsseq.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/p5_pbe.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/p5_pbev2.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/p5_scrypt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/p8_pkey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/t_bitst.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/t_pkey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/t_spki.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/tasn_dec.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/tasn_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/tasn_fre.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/tasn_new.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/tasn_prn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/tasn_scn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/tasn_typ.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/tasn_utl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_algor.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_bignum.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_info.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_int64.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_long.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_pkey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_sig.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_spki.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/asn1/x_val.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_add.c>
#if BOSS_WINDOWS & !BOSS_WINDOWS_MINGW
    #include <addon/openssl-1.1.1a_for_boss/crypto/bn/asm/x86_64-gcc.c> // assembly code
#else
    #include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_asm.c>
#endif
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_blind.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_const.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_ctx.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_depr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_dh.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_div.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_exp.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_exp2.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_gcd.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_gf2m.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_intern.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_kron.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_mod.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_mont.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_mpi.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_mul.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_nist.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_prime.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_print.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_rand.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_recp.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_shift.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_sqr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_sqrt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_srp.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_word.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/bn_x931p.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/bn/rsaz_exp.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_all.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_info.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_oth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_pk8.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_pkey.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_sign.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_x509.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pem_xaux.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/pem/pvkfmt.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/rand/drbg_ctr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rand/drbg_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rand/rand_egd.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rand/rand_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/rand/rand_lib.c>
#if BOSS_WINDOWS & !BOSS_NEED_FORCED_FAKEWIN
    #include <addon/openssl-1.1.1a_for_boss/crypto/rand/rand_unix.c>
    #include <addon/openssl-1.1.1a_for_boss/crypto/rand/rand_vms.c>
    #include <addon/openssl-1.1.1a_for_boss/crypto/rand/rand_win.c>
#elif BOSS_MAC_OSX | BOSS_IPHONE
    static uint64_t get_time_stamp(void)
    {
        return time(NULL);
    }
    static uint64_t get_timer_bits(void)
    {
        uint64_t res = OPENSSL_rdtsc();
        if (res != 0)
            return res;
        return time(NULL);
    }
    int rand_pool_init(void)
    {
        return 1;
    }
    void rand_pool_cleanup(void)
    {
    }
    void rand_pool_keep_random_devices_open(int keep)
    {
    }
    size_t rand_pool_acquire_entropy(RAND_POOL *pool)
    {
        return rand_pool_entropy_available(pool);
    }
    int rand_pool_add_nonce_data(RAND_POOL *pool)
    {
        struct {
            pid_t pid;
            CRYPTO_THREAD_ID tid;
            uint64_t time;
        } data = { 0 };

        /*
         * Add process id, thread id, and a high resolution timestamp to
         * ensure that the nonce is unique with high probability for
         * different process instances.
         */
        data.pid = getpid();
        data.tid = CRYPTO_THREAD_get_current_id();
        data.time = get_time_stamp();

        return rand_pool_add(pool, (unsigned char *)&data, sizeof(data), 0);
    }
    int rand_pool_add_additional_data(RAND_POOL *pool)
    {
        struct {
            CRYPTO_THREAD_ID tid;
            uint64_t time;
        } data = { 0 };

        /*
         * Add some noise from the thread id and a high resolution timer.
         * The thread id adds a little randomness if the drbg is accessed
         * concurrently (which is the case for the <master> drbg).
         */
        data.tid = CRYPTO_THREAD_get_current_id();
        data.time = get_timer_bits();

        return rand_pool_add(pool, (unsigned char *)&data, sizeof(data), 0);
    }
#else
    unsigned int arc4random()
    {
        return rand() & 0xFFFFFFFF;
    }
    int RAND_poll(void)
    {
        unsigned int rnd = 0, i;
        unsigned char buf[ENTROPY_NEEDED];

        for (i = 0; i < sizeof(buf); i++) {
            if (i % 4 == 0)
                rnd = arc4random();
            buf[i] = rnd;
            rnd >>= 8;
        }
        RAND_add(buf, sizeof(buf), ENTROPY_NEEDED);
        memset(buf, 0, sizeof(buf));

        return 1;
    }
#endif
void rc4_md5_enc(RC4_KEY *key, const void *in0, void *out,
    MD5_CTX *ctx, const void *inp, size_t blocks)
{
    BOSS_ASSERT("rc4_md5_enc호출은 추가개발필요", 0);
}
#include <addon/openssl-1.1.1a_for_boss/crypto/rand/randfile.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/stack/stack.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/buffer/buffer.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/buffer/buf_err.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/lhash/lh_stats.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/lhash/lhash.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/conf/conf_api.c>
#define str_copy str_copy_conf_def_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/conf/conf_def.c>
#undef str_copy
#include <addon/openssl-1.1.1a_for_boss/crypto/conf/conf_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/conf/conf_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/conf/conf_mall.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/conf/conf_mod.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/conf/conf_sap.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/conf/conf_ssl.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/dso/dso_dl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dso/dso_dlfcn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dso/dso_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dso/dso_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dso/dso_openssl.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dso/dso_vms.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dso/dso_win32.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/cmac/cmac.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/cmac/cm_ameth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/cmac/cm_pmeth.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/hmac/hmac.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/hmac/hm_ameth.c>
#define int_update int_update_hm_pmeth_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/hmac/hm_pmeth.c>
#undef int_update

#include <addon/openssl-1.1.1a_for_boss/crypto/ripemd/rmd_dgst.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ripemd/rmd_one.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/whrlpool/wp_dgst.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/whrlpool/wp_block.c>
#undef N

#include <addon/openssl-1.1.1a_for_boss/crypto/modes/cbc128.c>
#define ctr64_inc ctr64_inc_ccm128_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/ccm128.c>
#undef ctr64_inc
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/cfb128.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/ctr128.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/cts128.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/gcm128.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/ocb128.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/ofb128.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/wrap128.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/modes/xts128.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_cbc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_cfb.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_core.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_ecb.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_ige.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_misc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_ofb.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_wrap.c>
#define Td4 Td4_aes_x86core_BOSS
#define rcon rcon_aes_x86core_BOSS
#define AES_set_encrypt_key AES_set_encrypt_key_aes_x86core_BOSS
#define AES_set_decrypt_key AES_set_decrypt_key_aes_x86core_BOSS
#define AES_encrypt AES_encrypt_aes_x86core_BOSS
#define AES_decrypt AES_decrypt_aes_x86core_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/aes/aes_x86core.c>
#undef Td4
#undef rcon
#undef AES_set_encrypt_key
#undef AES_set_decrypt_key
#undef AES_encrypt
#undef AES_decrypt

#include <addon/openssl-1.1.1a_for_boss/crypto/async/async_wait.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/async/async.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/async/async_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/async/arch/async_win.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/srp/srp_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/srp/srp_vfy.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_ameth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_check.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_depr.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_gen.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_kdf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_key.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_meth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_pmeth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_prn.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_rfc5114.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/dh/dh_rfc7919.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/comp/comp_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/comp/c_zlib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/comp/comp_err.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/des/cbc_cksm.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/cbc_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/cfb64ede.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/cfb64enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/cfb_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/des_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/ecb3_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/ecb_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/fcrypt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/fcrypt_b.c>
#define DES_ncbc_encrypt DES_ncbc_encrypt_ncbc_enc_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/des/ncbc_enc.c>
#undef DES_ncbc_encrypt
#include <addon/openssl-1.1.1a_for_boss/crypto/des/ofb64ede.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/ofb64enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/ofb_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/pcbc_enc.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/qud_cksm.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/rand_key.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/set_key.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/str2key.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/des/xcbc_enc.c>

#define init init_m_blake2b_BOSS
#define update update_m_blake2b_BOSS
#define final final_m_blake2b_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/blake2/m_blake2b.c>
#undef init
#undef update
#undef final
#include <addon/openssl-1.1.1a_for_boss/crypto/blake2/blake2b.c>
#define init init_m_blake2s_BOSS
#define update update_m_blake2s_BOSS
#define final final_m_blake2s_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/blake2/m_blake2s.c>
#undef init
#undef update
#undef final
#include <addon/openssl-1.1.1a_for_boss/crypto/blake2/blake2s.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/kdf/hkdf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/kdf/kdf_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/kdf/scrypt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/kdf/tls1_prf.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/chacha/chacha_enc.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/poly1305/poly1305.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/poly1305/poly1305_ameth.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/poly1305/poly1305_base2_44.c>
//#include <addon/openssl-1.1.1a_for_boss/crypto/poly1305/poly1305_ieee754.c>
#define int_update int_update_poly1305_pmeth_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/poly1305/poly1305_pmeth.c>
#undef int_update

#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_conf.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_req_print.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_req_utils.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_rsp_print.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_rsp_sign.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_rsp_utils.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_rsp_verify.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/ts/ts_verify_ctx.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/txt_db/txt_db.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/sm2/sm2_crypt.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/sm2/sm2_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/sm2/sm2_pmeth.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/sm2/sm2_sign.c>

#define init init_m_sm3_BOSS
#define update update_m_sm3_BOSS
#define final final_m_sm3_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/sm3/m_sm3.c>
#undef init
#undef update
#undef final
#include <addon/openssl-1.1.1a_for_boss/crypto/sm3/sm3.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/sm4/sm4.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/siphash/siphash.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/siphash/siphash_ameth.c>
#define int_update int_update_siphash_pmeth_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/siphash/siphash_pmeth.c>
#undef int_update

#define file_ctrl file_ctrl_loader_file_BOSS
#include <addon/openssl-1.1.1a_for_boss/crypto/store/loader_file.c>
#undef file_ctrl
#include <addon/openssl-1.1.1a_for_boss/crypto/store/store_err.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/store/store_init.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/store/store_lib.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/store/store_register.c>
#include <addon/openssl-1.1.1a_for_boss/crypto/store/store_strings.c>

#include <addon/openssl-1.1.1a_for_boss/crypto/aria/aria.c>

#include <addon/openssl-1.1.1a_for_boss/ssl/bio_ssl.c>
#define get_current_time get_current_time_d1_lib_BOSS
#include <addon/openssl-1.1.1a_for_boss/ssl/d1_lib.c>
#undef get_current_time
#include <addon/openssl-1.1.1a_for_boss/ssl/d1_msg.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/d1_srtp.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/methods.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/packet.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/pqueue.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/s3_cbc.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/s3_enc.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/s3_lib.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/s3_msg.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_asn1.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_cert.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_ciph.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_conf.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_err.c>
#define stopped stopped_ssl_init_BOSS
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_init.c>
#undef stopped
#define ssl_undefined_function_1 ssl_undefined_function_1_ssl_lib_BOSS
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_lib.c>
#undef ssl_undefined_function_1
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_mcnf.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_rsa.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_sess.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_stat.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_txt.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/ssl_utst.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/t1_enc.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/t1_lib.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/t1_trce.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/tls13_enc.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/tls_srp.c>

#include <addon/openssl-1.1.1a_for_boss/ssl/record/dtls1_bitmap.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/record/rec_layer_d1.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/record/rec_layer_s3.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/record/ssl3_buffer.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/record/ssl3_record.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/record/ssl3_record_tls13.c>

#include <addon/openssl-1.1.1a_for_boss/ssl/statem/extensions.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/statem/extensions_clnt.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/statem/extensions_cust.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/statem/extensions_srvr.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/statem/statem.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/statem/statem_clnt.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/statem/statem_dtls.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/statem/statem_lib.c>
#include <addon/openssl-1.1.1a_for_boss/ssl/statem/statem_srvr.c>

#endif
