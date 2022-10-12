
/* -*- c-basic-offset: 4 -*- */
#include <pocketsphinx.h>

#include "ps_alignment.h"
#include "state_align_search.h"
#include "pocketsphinx_internal.h"

#include "test_macros.h"

static int
do_search(ps_search_t *search, acmod_t *acmod)
{
    FILE *rawfh;
    int16 buf[2048];
    size_t nread;
    int16 const *bptr;
    int nfr;

    TEST_ASSERT(rawfh = fopen(DATADIR "/goforward.raw", "rb"));
    TEST_EQUAL(0, acmod_start_utt(acmod));
    ps_search_start(search);
    while (!feof(rawfh)) {
        nread = fread(buf, sizeof(*buf), 2048, rawfh);
        bptr = buf;
        while ((nfr = acmod_process_raw(acmod, &bptr, &nread, FALSE)) > 0) {
            while (acmod->n_feat_frame > 0) {
                ps_search_step(search, acmod->output_frame);
                acmod_advance(acmod);
            }
        }
    }
    TEST_ASSERT(acmod_end_utt(acmod) >= 0);
    fclose(rawfh);
    return ps_search_finish(search);
}

static int
do_decode(ps_decoder_t *ps)
{
    FILE *rawfh;
    const char *hyp;
    long nsamp;
    int score;
    
    TEST_ASSERT(rawfh = fopen(DATADIR "/goforward.raw", "rb"));
    nsamp = ps_decode_raw(ps, rawfh, -1);
    hyp = ps_get_hyp(ps, &score);
    printf("%s (%ld samples, %d score)\n", hyp, nsamp, score);
    TEST_ASSERT(nsamp > 0);
    TEST_EQUAL(0, strcmp(hyp, "<s> go forward ten meters </s>"));
    fclose(rawfh);

    return 0;
}

int
main(int argc, char *argv[])
{
    ps_decoder_t *ps;
    dict_t *dict;
    dict2pid_t *d2p;
    acmod_t *acmod;
    ps_alignment_t *al;
    ps_alignment_iter_t *itor;
    ps_search_t *search;
    ps_seg_t *seg;
    cmd_ln_t *config;
    int i, sf, ef, last_ef;

    (void)argc;
    (void)argv;
    TEST_ASSERT(config =
                ps_config_parse_json(
                    NULL,
                    "hmm: \"" MODELDIR "/en-us/en-us\","
                    "dict: \"" MODELDIR "/en-us/cmudict-en-us.dict\","
                    "samprate: 16000"));
    TEST_ASSERT(ps = ps_init(config));
    dict = ps->dict;
    d2p = ps->d2p;
    acmod = ps->acmod;

    al = ps_alignment_init(d2p);
    TEST_EQUAL(1, ps_alignment_add_word(al, dict_wordid(dict, "<s>"), 0));
    TEST_EQUAL(2, ps_alignment_add_word(al, dict_wordid(dict, "go"), 0));
    TEST_EQUAL(3, ps_alignment_add_word(al, dict_wordid(dict, "forward"), 0));
    TEST_EQUAL(4, ps_alignment_add_word(al, dict_wordid(dict, "ten"), 0));
    TEST_EQUAL(5, ps_alignment_add_word(al, dict_wordid(dict, "meters"), 0));
    TEST_EQUAL(6, ps_alignment_add_word(al, dict_wordid(dict, "</s>"), 0));
    TEST_EQUAL(0, ps_alignment_populate(al));

    TEST_ASSERT(search = state_align_search_init("state_align", config, acmod, al));

    for (i = 0; i < 5; i++)
        do_search(search, acmod);

    for (itor = ps_alignment_words(al); itor;
	 itor = ps_alignment_iter_next(itor)) {
	ps_alignment_entry_t *ent = ps_alignment_iter_get(itor);

	printf("%s %d %d\n",
	       dict_wordstr(dict, ent->id.wid),
	       ent->start, ent->duration);
    }
    itor = ps_alignment_words(al);
    TEST_EQUAL(ps_alignment_iter_get(itor)->start, 0);
    TEST_EQUAL(ps_alignment_iter_get(itor)->duration, 46);
    itor = ps_alignment_iter_next(itor);
    TEST_EQUAL(ps_alignment_iter_get(itor)->start, 46);
    TEST_EQUAL(ps_alignment_iter_get(itor)->duration, 18);
    itor = ps_alignment_iter_next(itor);
    TEST_EQUAL(ps_alignment_iter_get(itor)->start, 64);
    TEST_EQUAL(ps_alignment_iter_get(itor)->duration, 53);
    itor = ps_alignment_iter_next(itor);
    TEST_EQUAL(ps_alignment_iter_get(itor)->start, 117);
    TEST_EQUAL(ps_alignment_iter_get(itor)->duration, 36);
    itor = ps_alignment_iter_next(itor);
    TEST_EQUAL(ps_alignment_iter_get(itor)->start, 153);
    TEST_EQUAL(ps_alignment_iter_get(itor)->duration, 59);
    itor = ps_alignment_iter_next(itor);
    TEST_EQUAL(ps_alignment_iter_get(itor)->start, 212);
    TEST_EQUAL(ps_alignment_iter_get(itor)->duration, 62);
    itor = ps_alignment_iter_next(itor);
    TEST_EQUAL(itor, NULL);

    ps_search_free(search);
    ps_alignment_free(al);

    /* Test alignment through the decoder/search API */
    TEST_EQUAL(0, ps_set_align(ps, "align", "go forward ten meters"));
    ps_set_search(ps, "align");
    do_decode(ps);
    TEST_EQUAL(0, strcmp(ps_get_hyp(ps, &i), "<s> go forward ten meters </s>"));
    seg = ps_seg_iter(ps);
    ps_seg_frames(seg, &sf, &ef);
    printf("%s %d %d\n", ps_seg_word(seg), sf, ef);
    TEST_EQUAL(0, strcmp("<s>", ps_seg_word(seg)));
    TEST_ASSERT(ef > sf);
    last_ef = ef;
    seg = ps_seg_next(seg);
    ps_seg_frames(seg, &sf, &ef);
    printf("%s %d %d\n", ps_seg_word(seg), sf, ef);
    TEST_EQUAL(0, strcmp("go", ps_seg_word(seg)));
    TEST_ASSERT(sf > last_ef);
    TEST_ASSERT(ef > sf);
    last_ef = ef;
    seg = ps_seg_next(seg);
    ps_seg_frames(seg, &sf, &ef);
    printf("%s %d %d\n", ps_seg_word(seg), sf, ef);
    TEST_EQUAL(0, strcmp("forward", ps_seg_word(seg)));
    TEST_ASSERT(sf > last_ef);
    TEST_ASSERT(ef > sf);
    last_ef = ef;
    seg = ps_seg_next(seg);
    ps_seg_frames(seg, &sf, &ef);
    printf("%s %d %d\n", ps_seg_word(seg), sf, ef);
    TEST_EQUAL(0, strcmp("ten", ps_seg_word(seg)));
    TEST_ASSERT(sf > last_ef);
    TEST_ASSERT(ef > sf);
    last_ef = ef;
    seg = ps_seg_next(seg);
    ps_seg_frames(seg, &sf, &ef);
    printf("%s %d %d\n", ps_seg_word(seg), sf, ef);
    TEST_EQUAL(0, strcmp("meters", ps_seg_word(seg)));
    TEST_ASSERT(sf > last_ef);
    TEST_ASSERT(ef > sf);
    last_ef = ef;
    seg = ps_seg_next(seg);
    ps_seg_frames(seg, &sf, &ef);
    printf("%s %d %d\n", ps_seg_word(seg), sf, ef);
    TEST_EQUAL(0, strcmp("</s>", ps_seg_word(seg)));
    TEST_ASSERT(sf > last_ef);
    TEST_ASSERT(ef > sf);
    last_ef = ef;
    seg = ps_seg_next(seg);
    TEST_EQUAL(NULL, seg);

    ps_free(ps);
    ps_config_free(config);
    return 0;
}
