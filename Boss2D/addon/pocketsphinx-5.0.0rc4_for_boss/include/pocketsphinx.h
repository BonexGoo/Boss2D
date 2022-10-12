/* -*- c-basic-offset:4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1999-2008 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */
/**
 * @file pocketsphinx.h Main header file for the PocketSphinx decoder.
 */

#ifndef __POCKETSPHINX_H__
#define __POCKETSPHINX_H__

/* System headers */
#include <stdio.h>

/* PocketSphinx utility headers */
#include <pocketsphinx/sphinx_config.h>
#include <pocketsphinx/prim_type.h>
#include <pocketsphinx/logmath.h>
#include <pocketsphinx/err.h>

/* PocketSphinx API headers */
#include <pocketsphinx/vad.h>
#include <pocketsphinx/endpointer.h>
#include <pocketsphinx/model.h>
#include <pocketsphinx/search.h>
#include <pocketsphinx/export.h>
#include <pocketsphinx/lattice.h>
#include <pocketsphinx/mllr.h>

/* Namum manglium ii domum */
#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* Transparent structures */
/**
 * @enum ps_type_t
 * Types of configuration parameters.
 */
typedef enum ps_type_e  {
    ARG_REQUIRED =  (1<<0), /*<< Bit indicating required argument. */
    ARG_INTEGER = (1<<1),   /*<< Integer up to 64 bits. */
    ARG_FLOATING  = (1<<2), /*<< Double-precision floating point. */
    ARG_STRING = (1<<3),    /*<< String. */
    ARG_BOOLEAN = (1<<4),   /*<< Boolean (true/false). */
    REQARG_INTEGER = (ARG_INTEGER | ARG_REQUIRED),
    REQARG_FLOATING = (ARG_FLOATING | ARG_REQUIRED),
    REQARG_STRING = (ARG_STRING | ARG_REQUIRED),
    REQARG_BOOLEAN = (ARG_BOOLEAN | ARG_REQUIRED)
} ps_type_t;

/**
 * @struct ps_arg_t
 * Definition of configuration parameter.
 */
typedef struct arg_s {
	char const *name;   /**< Name of the command line switch */
	int type;           /**< Type of the argument in question */
	char const *deflt;  /**< Default value (as a character string), or NULL if none */
	char const *doc;    /**< Documentation/description string */
} arg_t;

/* Opaque structures */

/**
 * @struct ps_config_t
 * PocketSphinx configuration object.
 */
typedef struct cmd_ln_s ps_config_t;

/**
 * @struct ps_decoder_t
 * PocketSphinx speech recognizer object.
 */
typedef struct ps_decoder_s ps_decoder_t;

/**
 * @struct ps_astar_t
 * PocketSphinx N-best hypothesis iterator object.
 */
typedef struct ps_astar_s ps_nbest_t;

/**
 * @struct ps_seg_t
 * PocketSphinx segmentation iterator object.
 */
typedef struct ps_seg_s ps_seg_t;

/**
 * Create a configuration with default values.
 *
 * @param defn Array of arg_t defining and describing parameters,
 * terminated by an arg_t with `name == NULL`.  You should usually
 * just pass NULL here, which will result in the standard set of
 * parameters being used.
 * @return Newly created configuration or NULL on failure (should not
 * happen, but check it anyway).
 */
POCKETSPHINX_EXPORT
ps_config_t *ps_config_init(const arg_t *defn);

/**
 * Retain a pointer to a configuration object.
 */
POCKETSPHINX_EXPORT
ps_config_t *ps_config_retain(ps_config_t *config);

/**
 * Release a configuration object.
 */
POCKETSPHINX_EXPORT
int ps_config_free(ps_config_t *config);

/**
 * Create or update a configuration by parsing slightly extended JSON.
 *
 * This function parses a JSON object in non-strict mode to produce a
 * ps_config_t.  Configuration parameters are given *without* a
 * leading dash, and do not need to be quoted, nor does the object
 * need to be enclosed in curly braces, nor are commas necessary
 * between key/value pairs.  Basically, it's degenerate YAML.  So, for
 * example, this is accepted:
 *
 *     hmm: fr-fr
 *     samprate: 8000
 *     keyprhase: "hello world"
 *
 * Of course, valid JSON is also accepted, but who wants to use that.
 *
 * @arg config Previously existing ps_config_t to update, or NULL to
 * create a new one.
 * @arg json JSON serialized object as null-terminated UTF-8,
 * containing configuration parameters.
 * @return Newly created configuration or NULL on failure (such as
 * invalid or missing parameters).
 */
POCKETSPHINX_EXPORT
ps_config_t *ps_config_parse_json(ps_config_t *config, const char *json);

/**
 * Construct JSON from a configuration object.
 *
 * Unlike ps_config_parse_json(), this actually produces valid JSON ;-)
 *
 * @arg config Configuration object
 * @return Newly created null-terminated JSON string.  The ps_config_t
 * retains ownership of this pointer, which is only valid until the
 * next call to ps_config_serialize_json().  You must copy it if you
 * wish to retain it.
 */
POCKETSPHINX_EXPORT
const char *ps_config_serialize_json(ps_config_t *config);

/**
 * Access the type of a configuration parameter.
 *
 * @param config Configuration object.
 * @param name Name of the parameter to retrieve.
 * @return the type of the parameter (as a combination of the ARG_*
 *         bits), or 0 if no such parameter exists.
 */
POCKETSPHINX_EXPORT
ps_type_t ps_config_typeof(ps_config_t *config, char const *name);

/**
 * Access the value of a configuration parameter.
 *
 * To actually do something with the value, you will need to know its
 * type, which can be obtained with ps_config_typeof().  This function
 * is thus mainly useful for dynamic language bindings, and you should
 * use ps_config_int(), ps_config_float(), or ps_config_str() instead.
 *
 * @param config Configuration object.
 * @param name Name of the parameter to retrieve.
 * @return Pointer to the parameter's value, or NULL if the parameter
 * does not exist.  Note that a string parameter can also have NULL as
 * a value, in which case the `ptr` field in the return value is NULL.
 * This pointer (and any pointers inside it) is owned by the ps_config_t.
 */
POCKETSPHINX_EXPORT
const anytype_t *ps_config_get(ps_config_t *config, const char *name);

/**
 * Set or unset the value of a configuration parameter.
 *
 * This will coerce the value to the proper type, so you can, for
 * example, pass it a string with ARG_STRING as the type when adding
 * options from the command-line.  Note that the return pointer will
 * *not* be the same as the one passed in the value.
 *
 * @param config Configuration object.
 * @param name Name of the parameter to set.  Must exist.
 * @param val Pointer to the value (strings will be copied) inside an
 * anytype_t union.  On 64-bit little-endian platforms, you *can* cast
 * a pointer to int, long, double, or char* here, but that doesn't
 * necessarily mean that you *should*.  As a convenience, you can pass
 * NULL here to reset a parameter to its default value.
 * @param t Type of the value in `val`, will be coerced to the type of
 * the actual parameter if necessary.
 * @return Pointer to the parameter's value, or NULL on failure
 * (unknown parameter, usually).  This pointer (and any pointers
 * inside it) is owned by the ps_config_t.
 */
POCKETSPHINX_EXPORT
const anytype_t *ps_config_set(ps_config_t *config, const char *name,
                               const anytype_t *val, ps_type_t t);

/**
 * Get an integer-valued parameter.
 *
 * If the parameter does not have an integer or boolean type, this
 * will print an error and return 0.  So don't do that.
 */
POCKETSPHINX_EXPORT
long ps_config_int(ps_config_t *config, const char *name);

/**
 * Get a boolean-valued parameter.
 *
 * If the parameter does not have an integer or boolean type, this
 * will print an error and return 0.  The return value is either 0 or
 * 1 (if the parameter has an integer type, any non-zero value will
 * return 1).
 */
POCKETSPHINX_EXPORT
int ps_config_bool(ps_config_t *config, const char *name);

/**
 * Get a floating-point parameter.
 *
 * If the parameter does not have a floating-point type, this will
 * print an error and return 0.
 */
POCKETSPHINX_EXPORT
double ps_config_float(ps_config_t *config, const char *name);

/**
 * Get a string parameter.
 *
 * If the parameter does not have a string type, this will print an
 * error and return NULL.  Notably, it will *NOT* format an integer or
 * float for you, because that would involve allocating memory.  So
 * don't do that.
 */
POCKETSPHINX_EXPORT
const char *ps_config_str(ps_config_t *config, const char *name);

/**
 * Set an integer-valued parameter.
 *
 * If the parameter does not have an integer or boolean type, this
 * will convert `val` appropriately.
 */
POCKETSPHINX_EXPORT
const anytype_t *ps_config_set_int(ps_config_t *config, const char *name, long val);

/**
 * Set a boolean-valued parameter.
 *
 * If the parameter does not have an integer or boolean type, this
 * will convert `val` appropriately.
 */
POCKETSPHINX_EXPORT
const anytype_t *ps_config_set_bool(ps_config_t *config, const char *name, int val);

/**
 * Set a floating-point parameter.
 *
 * If the parameter does not have a floating-point type, this will
 * convert `val` appropriately.
 */
POCKETSPHINX_EXPORT
const anytype_t *ps_config_set_float(ps_config_t *config, const char *name, double val);

/**
 * Set a string-valued parameter.
 *
 * If the parameter does not have a string type, this will convert
 * `val` appropriately.  For boolean parameters, any string matching
 * `/^[yt1]/` will be true, while any string matching `/^[nf0]/` will
 * be false.  NULL is also false.
 *
 * This function is used for configuration from JSON, you may want to
 * use it for your own configuration files too.
 */
POCKETSPHINX_EXPORT
const anytype_t *ps_config_set_str(ps_config_t *config, const char *name, const char *val);

/**
 * Sets default grammar and language model if they are not set explicitly and
 * are present in the default search path.
 */
POCKETSPHINX_EXPORT
void ps_default_search_args(ps_config_t *config);

/**
 * Sets default file paths and parameters based on configuration.
 */
POCKETSPHINX_EXPORT
void ps_expand_model_config(ps_config_t *config);

/**
 * Gets the system default model directory, if any exists.
 *
 * @return system model directory defined at compile time, or NULL if
 *         not defined (usually in a relocatable installation such as
 *         a Python module).
 */
POCKETSPHINX_EXPORT
const char *ps_default_modeldir(void);

/**
 * Initialize the decoder from a configuration object.
 *
 * @note The decoder retains ownership of the pointer
 * <code>config</code>, so if you are not going to use it
 * elsewhere, you can free it.
 * @param config a configuration object.  If NULL, the
 * decoder will be allocated but not initialized.  You can
 * proceed to initialize it with ps_reinit().
 */
POCKETSPHINX_EXPORT
ps_decoder_t *ps_init(ps_config_t *config);

/**
 * Reinitialize the decoder with updated configuration.
 *
 * This function allows you to switch the acoustic model, dictionary,
 * or other configuration without creating an entirely new decoding
 * object.
 *
 * @note Since the acoustic model will be reloaded, changes made to
 * feature extraction parameters may be overridden if a `feat.params`
 * file is present.
 * @note Any searches created with ps_set_search() or words added to
 * the dictionary with ps_add_word() will also be lost.  To avoid this
 * you can use ps_reinit_feat().
 * @note The decoder retains ownership of the pointer
 * <code>config</code>, so you should free it when no longer used.
 *
 * @param ps Decoder.
 * @param config An optional new configuration to use.  If this is
 *               NULL, the previous configuration will be reloaded,
 *               with any changes applied.
 * @return 0 for success, <0 for failure.
 */
POCKETSPHINX_EXPORT
int ps_reinit(ps_decoder_t *ps, ps_config_t *config);

/**
 * Reinitialize only the feature computation with updated configuration.
 *
 * This function allows you to switch the feature computation
 * parameters without otherwise affecting the decoder configuration.
 * For example, if you change the sample rate or the frame rate, and
 * do not want to reconfigure the rest of the decoder.
 *
 * Note that if your code has modified any internal parameters in the
 * \ref acmod_t, these will be overriden by values from the config.
 * Likewise if you have set a custom cepstral mean with ps_set_cmn(),
 * it will be overridden.
 *
 * @note The decoder retains ownership of the pointer `config`, so you
 * should free it when no longer used.
 *
 * @param ps Decoder.
 * @param config An optional new configuration to use.  If this is
 *               NULL, the previous configuration will be reloaded,
 *               with any changes to feature computation applied.
 * @return 0 for success, <0 for failure (usually an invalid parameter)
 */
POCKETSPHINX_EXPORT
int ps_reinit_feat(ps_decoder_t *ps, ps_config_t *config);

/**
 * Get the current cepstral mean as a string.
 *
 * This is the string representation of the current cepstral mean,
 * which represents the acoustic channel conditions in live
 * recognition.  This can be used to initialize the decoder with the
 * `-cmninit` flag.
 *
 * @param ps Decoder
 * @param update Update the cepstral mean using data processed so far.
 * @return String representation of cepstral mean, as
 *         `-ceplen` comma-separated numbers.  This pointer is owned
 *         by the decoder and only valid until the next call to
 *         ps_get_cmn(), ps_set_cmn() or ps_end_utt().
 */
POCKETSPHINX_EXPORT
const char *ps_get_cmn(ps_decoder_t *ps, int update);

/**
 * Set the current cepstral mean from a string.
 *
 * This does the same thing as setting `-cmninit` and running
 * `ps_reinit_feat()` but is more efficient, and can also be
 * done in the middle of an utterance if you like.
 *
 * @param ps Decoder
 * @param cmn String representation of cepstral mean, as
 *            up to `-ceplen` comma-separated numbers (any
 *            missing values will be zero-filled).
 * @return 0 for success of -1 for invalid input.
 */
POCKETSPHINX_EXPORT
int ps_set_cmn(ps_decoder_t *ps, const char *cmn);

/**
 * Returns the argument definitions used in ps_init().
 *
 * This is here to avoid exporting global data, which is problematic
 * on Win32 and Symbian (and possibly other platforms).
 */
POCKETSPHINX_EXPORT
arg_t const *ps_args(void);

/**
 * Retain a pointer to the decoder.
 *
 * This increments the reference count on the decoder, allowing it to
 * be shared between multiple parent objects.  In general you will not
 * need to use this function, ever.  It is mainly here for the
 * convenience of scripting language bindings.
 *
 * @return pointer to retained decoder.
 */
POCKETSPHINX_EXPORT
ps_decoder_t *ps_retain(ps_decoder_t *ps);

/**
 * Finalize the decoder.
 *
 * This releases all resources associated with the decoder.
 *
 * @param ps Decoder to be freed.
 * @return New reference count (0 if freed).
 */
POCKETSPHINX_EXPORT
int ps_free(ps_decoder_t *ps);

/**
 * Get the configuration object for this decoder.
 *
 * @return The configuration object for this decoder.  The decoder
 *         owns this pointer, so you should not attempt to free it
 *         manually.  Use ps_config_retain() if you wish to reuse it
 *         elsewhere.
 */
POCKETSPHINX_EXPORT
ps_config_t *ps_get_config(ps_decoder_t *ps);

/**
 * Get the log-math computation object for this decoder.
 *
 * @return The log-math object for this decoder.  The decoder owns
 *         this pointer, so you should not attempt to free it
 *         manually.  Use logmath_retain() if you wish to reuse it
 *         elsewhere.
 */
POCKETSPHINX_EXPORT
logmath_t *ps_get_logmath(ps_decoder_t *ps);

/**
 * Adapt current acoustic model using a linear transform.
 *
 * @param mllr The new transform to use, or NULL to update the
 *              existing transform.  The decoder retains ownership of
 *              this pointer, so you may free it if you no longer need
 *              it.
 * @return The updated transform object for this decoder, or
 *         NULL on failure.
 */
POCKETSPHINX_EXPORT
ps_mllr_t *ps_update_mllr(ps_decoder_t *ps, ps_mllr_t *mllr);

/**
 * Reload the pronunciation dictionary from a file.
 *
 * This function replaces the current pronunciation dictionary with
 * the one stored in `dictfile`.  This also causes the active search
 * module(s) to be reinitialized, in the same manner as calling
 * ps_add_word() with update=TRUE.
 *
 * @param dictfile Path to dictionary file to load.
 * @param fdictfile Path to filler dictionary to load, or NULL to keep
 *                  the existing filler dictionary.
 * @param format Format of the dictionary file, or NULL to determine
 *               automatically (currently unused,should be NULL)
 */
POCKETSPHINX_EXPORT
int ps_load_dict(ps_decoder_t *ps, char const *dictfile,
                 char const *fdictfile, char const *format);

/**
 * Dump the current pronunciation dictionary to a file.
 *
 * This function dumps the current pronunciation dictionary to a text file.
 *
 * @param dictfile Path to file where dictionary will be written.
 * @param format Format of the dictionary file, or NULL for the
 *               default (text) format (currently unused, should be NULL)
 */
POCKETSPHINX_EXPORT
int ps_save_dict(ps_decoder_t *ps, char const *dictfile, char const *format);

/**
 * Add a word to the pronunciation dictionary.
 *
 * This function adds a word to the pronunciation dictionary and the
 * current language model (but, obviously, not to the current FSG if
 * FSG mode is enabled).  If the word is already present in one or the
 * other, it does whatever is necessary to ensure that the word can be
 * recognized.
 *
 * @param word Word string to add.
 * @param phones Whitespace-separated list of phoneme strings
 *               describing pronunciation of <code>word</code>.
 * @param update If TRUE, update the search module (whichever one is
 *               currently active) to recognize the newly added word.
 *               If adding multiple words, it is more efficient to
 *               pass FALSE here in all but the last word.
 * @return The internal ID (>= 0) of the newly added word, or <0 on
 *         failure.
 */
POCKETSPHINX_EXPORT
int ps_add_word(ps_decoder_t *ps,
                char const *word,
                char const *phones,
                int update);

/** 
 * Look up a word in the dictionary and return phone transcription
 * for it.
 *
 * @param ps Pocketsphinx decoder
 * @param word Word to look for
 *
 * @return Whitespace-spearated phone string describing the pronunciation of the <code>word</code>
 *         or NULL if word is not present in the dictionary. The string is
 *         allocated and must be freed by the user.
 */
POCKETSPHINX_EXPORT
char *ps_lookup_word(ps_decoder_t *ps, 
	             const char *word);

/**
 * Decode a raw audio stream.
 *
 * No headers are recognized in this files.  The configuration
 * parameters <tt>-samprate</tt> and <tt>-input_endian</tt> are used
 * to determine the sampling rate and endianness of the stream,
 * respectively.  Audio is always assumed to be 16-bit signed PCM.
 *
 * @param ps Decoder.
 * @param rawfh Previously opened file stream.
 * @param maxsamps Maximum number of samples to read from rawfh, or -1
 *                 to read until end-of-file.
 * @return Number of samples of audio.
 */
POCKETSPHINX_EXPORT
long ps_decode_raw(ps_decoder_t *ps, FILE *rawfh,
                   long maxsamps);

/**
 * Decode a senone score dump file.
 *
 * @param ps Decoder
 * @param senfh Previously opened file handle positioned at start of file.
 * @return Number of frames read.
 */
POCKETSPHINX_EXPORT
int ps_decode_senscr(ps_decoder_t *ps, FILE *senfh);

/**
 * Start processing of the stream of speech.
 *
 * @deprecated This function is retained for compatibility, but its
 * only effect is to reset the noise removal statistics, which are
 * otherwise retained across utterances.  You do not need to call it.
 *
 * @return 0 for success, <0 on error.
 */
POCKETSPHINX_EXPORT
int ps_start_stream(ps_decoder_t *ps);

/**
 * Checks if the last feed audio buffer contained speech.
 *
 * @deprecated This function is retained for compatibility but should
 * not be considered a reliable voice activity detector.  It will
 * always return 1 between calls to ps_start_utt() and ps_end_utt().
 * You probably want ps_endpointer_t, but for single frames of data
 * you can also use ps_vad_t
 *
 * @param ps Decoder.
 * @return 1 if last buffer contained speech, 0 - otherwise
 */
POCKETSPHINX_EXPORT
int ps_get_in_speech(ps_decoder_t *ps);

/**
 * Start utterance processing.
 *
 * This function should be called before any utterance data is passed
 * to the decoder.  It marks the start of a new utterance and
 * reinitializes internal data structures.
 *
 * @param ps Decoder to be started.
 * @return 0 for success, <0 on error.
 */
POCKETSPHINX_EXPORT
int ps_start_utt(ps_decoder_t *ps);

/**
 * Decode raw audio data.
 *
 * @param ps Decoder.
 * @param no_search If non-zero, perform feature extraction but don't
 *                  do any recognition yet.  This may be necessary if
 *                  your processor has trouble doing recognition in
 *                  real-time.
 * @param full_utt If non-zero, this block of data is a full utterance
 *                 worth of data.  This may allow the recognizer to
 *                 produce more accurate results.
 * @return Number of frames of data searched, or <0 for error.
 */
POCKETSPHINX_EXPORT
int ps_process_raw(ps_decoder_t *ps,
                   int16 const *data,
                   size_t n_samples,
                   int no_search,
                   int full_utt);

/**
 * Decode acoustic feature data.
 *
 * @param ps Decoder.
 * @param data Acoustic feature data, as returned by... something :)
 * @param no_search If non-zero, perform feature extraction but don't
 *                  do any recognition yet.  This may be necessary if
 *                  your processor has trouble doing recognition in
 *                  real-time.
 * @param full_utt If non-zero, this block of data is a full utterance
 *                 worth of data.  This may allow the recognizer to
 *                 produce more accurate results.
 * @return Number of frames of data searched, or <0 for error.
 */
POCKETSPHINX_EXPORT
int ps_process_cep(ps_decoder_t *ps,
                   float **data,
                   int n_frames,
                   int no_search,
                   int full_utt);

/**
 * Get the number of frames of data searched.
 *
 * Note that there is a delay between this and the number of frames of
 * audio which have been input to the system.  This is due to the fact
 * that acoustic features are computed using a sliding window of
 * audio, and dynamic features are computed over a sliding window of
 * acoustic features.
 *
 * @param ps Decoder.
 * @return Number of frames of speech data which have been recognized
 * so far.
 */
POCKETSPHINX_EXPORT
int ps_get_n_frames(ps_decoder_t *ps);

/**
 * End utterance processing.
 *
 * @param ps Decoder.
 * @return 0 for success, <0 on error
 */
POCKETSPHINX_EXPORT
int ps_end_utt(ps_decoder_t *ps);

/**
 * Get hypothesis string and path score.
 *
 * @param ps Decoder.
 * @param out_best_score Output: path score corresponding to returned string.
 * @return String containing best hypothesis at this point in
 *         decoding.  NULL if no hypothesis is available.  This string is owned
 *         by the decoder and only valid for the current hypothesis, so you
 *         should copy it if you need to hold onto it.
 */
POCKETSPHINX_EXPORT
char const *ps_get_hyp(ps_decoder_t *ps, int32 *out_best_score);

/**
 * Get posterior probability.
 *
 * @note Unless the -bestpath option is enabled, this function will
 * always return zero (corresponding to a posterior probability of
 * 1.0).  Even if -bestpath is enabled, it will also return zero when
 * called on a partial result.  Ongoing research into effective
 * confidence annotation for partial hypotheses may result in these
 * restrictions being lifted in future versions.
 *
 * @param ps Decoder.
 * @return Posterior probability of the best hypothesis.
 */
POCKETSPHINX_EXPORT
int32 ps_get_prob(ps_decoder_t *ps);

/**
 * Get word lattice.
 *
 * There isn't much you can do with this so far, a public API will
 * appear in the future.
 *
 * @param ps Decoder.
 * @return Word lattice object containing all hypotheses so far.  NULL
 *         if no hypotheses are available.  This pointer is owned by
 *         the decoder and you should not attempt to free it manually.
 *         It is only valid until the next utterance, unless you use
 *         ps_lattice_retain() to retain it.
 */
POCKETSPHINX_EXPORT
ps_lattice_t *ps_get_lattice(ps_decoder_t *ps);

/**
 * Get an iterator over the word segmentation for the best hypothesis.
 *
 * @param ps Decoder.
 * @return Iterator over the best hypothesis at this point in
 *         decoding.  NULL if no hypothesis is available.
 */
POCKETSPHINX_EXPORT
ps_seg_t *ps_seg_iter(ps_decoder_t *ps);

/**
 * Get the next segment in a word segmentation.
 *
 * @param seg Segment iterator.
 * @return Updated iterator with the next segment.  NULL at end of
 *         utterance (the iterator will be freed in this case).
 */
POCKETSPHINX_EXPORT
ps_seg_t *ps_seg_next(ps_seg_t *seg);

/**
 * Get word string from a segmentation iterator.
 *
 * @param seg Segment iterator.
 * @return Read-only string giving string name of this segment.  This
 * is only valid until the next call to ps_seg_next().
 */
POCKETSPHINX_EXPORT
char const *ps_seg_word(ps_seg_t *seg);

/**
 * Get inclusive start and end frames from a segmentation iterator.
 *
 * @note These frame numbers are inclusive, i.e. the end frame refers
 * to the last frame in which the given word or other segment was
 * active.  Therefore, the actual duration is *out_ef - *out_sf + 1.
 *
 * @param seg Segment iterator.
 * @param out_sf Output: First frame index in segment.
 * @param out_ef Output: Last frame index in segment.
 */
POCKETSPHINX_EXPORT
void ps_seg_frames(ps_seg_t *seg, int *out_sf, int *out_ef);

/**
 * Get language, acoustic, and posterior probabilities from a
 * segmentation iterator.
 *
 * @note Unless the -bestpath option is enabled, this function will
 * always return zero (corresponding to a posterior probability of
 * 1.0).  Even if -bestpath is enabled, it will also return zero when
 * called on a partial result.  Ongoing research into effective
 * confidence annotation for partial hypotheses may result in these
 * restrictions being lifted in future versions.
 *
 * @param out_ascr Output: acoustic model score for this segment.
 * @param out_lscr Output: language model score for this segment.
 * @param out_lback Output: language model backoff mode for this
 *                  segment (i.e. the number of words used in
 *                  calculating lscr).  This field is, of course, only
 *                  meaningful for N-Gram models.
 * @return Log posterior probability of current segment.  Log is
 *         expressed in the log-base used in the decoder.  To convert
 *         to linear floating-point, use logmath_exp(ps_get_logmath(),
 *         pprob).
 */
POCKETSPHINX_EXPORT
int32 ps_seg_prob(ps_seg_t *seg, int32 *out_ascr, int32 *out_lscr, int32 *out_lback);

/**
 * Finish iterating over a word segmentation early, freeing resources.
 */
POCKETSPHINX_EXPORT
void ps_seg_free(ps_seg_t *seg);

/**
 * Get an iterator over the best hypotheses. The function may also
 * return a NULL which means that there is no hypothesis available for this
 * utterance.
 *
 * @param ps Decoder.
 * @return Iterator over N-best hypotheses or NULL if no hypothesis is available
 */
POCKETSPHINX_EXPORT
ps_nbest_t *ps_nbest(ps_decoder_t *ps);

/**
 * Move an N-best list iterator forward.
 *
 * @param nbest N-best iterator.
 * @return Updated N-best iterator, or NULL if no more hypotheses are
 *         available (iterator is freed ni this case).
 */
POCKETSPHINX_EXPORT 
ps_nbest_t *ps_nbest_next(ps_nbest_t *nbest);

/**
 * Get the hypothesis string from an N-best list iterator.
 *
 * @param nbest N-best iterator.
 * @param out_score Output: Path score for this hypothesis.
 * @return String containing next best hypothesis. Note that this
 *         pointer is only valid for the current iteration.
 */
POCKETSPHINX_EXPORT
char const *ps_nbest_hyp(ps_nbest_t *nbest, int32 *out_score);

/**
 * Get the word segmentation from an N-best list iterator.
 *
 * @param nbest N-best iterator.
 * @return Iterator over the next best hypothesis.
 */
POCKETSPHINX_EXPORT
ps_seg_t *ps_nbest_seg(ps_nbest_t *nbest);

/**
 * Finish N-best search early, releasing resources.
 *
 * @param nbest N-best iterator.
 */
POCKETSPHINX_EXPORT
void ps_nbest_free(ps_nbest_t *nbest);

/**
 * Get performance information for the current utterance.
 *
 * @param ps Decoder.
 * @param out_nspeech Output: Number of seconds of speech.
 * @param out_ncpu    Output: Number of seconds of CPU time used.
 * @param out_nwall   Output: Number of seconds of wall time used.
 */
POCKETSPHINX_EXPORT
void ps_get_utt_time(ps_decoder_t *ps, double *out_nspeech,
                     double *out_ncpu, double *out_nwall);

/**
 * Get overall performance information.
 *
 * @param ps Decoder.
 * @param out_nspeech Output: Number of seconds of speech.
 * @param out_ncpu    Output: Number of seconds of CPU time used.
 * @param out_nwall   Output: Number of seconds of wall time used.
 */
POCKETSPHINX_EXPORT
void ps_get_all_time(ps_decoder_t *ps, double *out_nspeech,
                     double *out_ncpu, double *out_nwall);

/**
 * @mainpage PocketSphinx API Documentation
 * @author David Huggins-Daines <dhdaines@gmail.com>
 * @version 5.0.0rc4
 * @date September 16, 2022
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for the PocketSphinx speech recognition
 * engine.  The main API calls are documented in <pocketsphinx.h>.
 *
 * @section install_sec Installation
 *
 * To install from source, you will need a C compiler and a recent
 * version of CMake.  If you wish to use an integrated development
 * environment, Visual Studio Code will automate most of this process
 * for you once you have installed C++ and CMake support as described
 * at https://code.visualstudio.com/docs/languages/cpp
 *
 * @subsection python_install Python module install
 *
 * The easiest way to program PocketSphinx is with the Python module.
 * This can be installed in a
 * [VirtualEnv](https://docs.python.org/3/library/venv.html) or
 * [Conda](https://docs.conda.io/projects/conda/en/latest/user-guide/concepts/environments.html)
 * environment without affecting the rest of your system.  For
 * example, from the *top-level source directory*:
 *
 *     python3 -m venv ~/ve_pocketsphinx
 *     . ~/ve_pocketsphinx/bin/activate
 *     pip install .
 *
 * There is no need to create a separate build directory as `pip` will
 * do this for you.
 *
 * @subsection unix_install Unix-like systems
 *
 * From the top-level source directory, use CMake to generate a build
 * directory:
 *
 *     cmake -S . -B build
 *
 * Now you can compile and run the tests, and install the code:
 *
 *     cmake --build build
 *     cmake --build build --target check
 *     cmake --build build --target install
 *
 * By default CMake will try to install things in `/usr/local`, which
 * you might not have access to.  If you want to install somewhere
 * else you need to set `CMAKE_INSTALL_PREFIX` *when running cmake for
 * the first time*, for example:
 *
 *     cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$HOME/.local
 *
 * In this case you may also need to set the `LD_LIBRARY_PATH`
 * environment variable so that the PocketSphinx library can be found:
 *
 *     export LD_LIBRARY_PATH=$HOME/.local/lib
 *
 * @subsection windows_install Windows
 *
 * On Windows, the process is similar, but you will need to tell CMake
 * what build tool you are using with the `-G` option, and there are
 * many of them.  The build is known to work with `nmake` but it is
 * easiest just to use Visual Studio Code, which should automatically
 * detect and offer to run the build when you add the source directory
 * to your list of directories.  Once built, you will find the DLL and
 * EXE files in `build\Debug` or `build\Release` depending on your
 * build type.  If the EXE files do not run, you need to ensure that
 * `pocketsphinx.dll` is located in the same directory as them.
 *
 * @section programming_sec Using the Library
 *
 * The Python interface is documented at
 * http://pocketsphinx.readthedocs.io/, where you will find a quick
 * start guide as well as a full API reference.
 *
 * @section faq_sec Frequently Asked Questions
 *
 * @subsection faq_audio Why doesn't my audio device work?
 *
 * Because it's an audio device.  They don't work, at least for things
 * other than making annoying "beep boop" noises and playing Video
 * Games.  More generally, I cannot solve this problem for you,
 * because every single computer, operating system, sound card,
 * microphone, phase of the moon, and day of the week is different
 * when it comes to recording audio.  That's why I suggest you use
 * SoX, because (a) it usually works, and (b) whoever wrote it seems
 * to have retired long ago, so you can't bother them.
 *
 * @subsection faq_error The recognized text is wrong.
 *
 * That's not a question!  But since this isn't Jeopardy, and my name
 * is not Watson, I'll try to answer it anyway.  Be aware that the
 * answer depends on many things, first and foremost what you mean by
 * "wrong".
 *
 * If it *sounds* the same, e.g. "wreck a nice beach" when you said
 * "recognize speech" then the issue is that the **language model** is
 * not appropriate for the task, domain, dialect, or whatever it is
 * you're trying to recognize.  You may wish to consider writing a
 * JSGF grammar and using it instead of the default language model
 * (with the `-jsgf` flag).  Or you can get an N-best list or word
 * lattice and rescore it with a better language model, such as a
 * recurrent neural network or a human being.
 *
 * If it is total nonsense, or if it is just blank, or if it's the
 * same word repeated, e.g. "a a a a a a", then there is likely a
 * problem with the input audio.  The sampling rate could be wrong, or
 * even if it's correct, you may have narrow-band data.  Try to look
 * at the spectrogram (Audacity can show you this) and see if it looks
 * empty or flat below the frequency in the `-upperf` flag.
 * Alternately it could just be very noisy.  In particular, if the
 * noise consists of other people talking, automatic speech
 * recognition will nearly always fail.
 *
 * @subsection faq_tech Why don't you support (pick one or more: WFST,
 * fMLLR, SAT, DNN, CTC, LAS, CNN, RNN, LSTM, etc)?
 *
 * Not because there's anything wrong with those things (except LAS,
 * which is kind of a dumb idea) but simply because PocketSphinx does
 * not do them, or anything like them, and there is no point in adding
 * them to it when other systems exist.  Many of them are also heavily
 * dependent on distasteful and wasteful platforms like C++, CUDA,
 * TensorFlow, PyTorch, and so on.
 *
 * @section thanks_sec Acknowledgements
 *
 * PocketSphinx was originally released by David Huggins-Daines, but
 * is largely based on the previous Sphinx-II and Sphinx-III systems,
 * developed by a large number of contributors at Carnegie Mellon
 * University, and released as open source under a BSD-like license
 * thanks to Kevin Lenzo.  For some time, it was maintained by
 * Nickolay Shmyrev and others at Alpha Cephei, Inc.  See the
 * `AUTHORS` file for a list of contributors.
 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __POCKETSPHINX_H__ */
