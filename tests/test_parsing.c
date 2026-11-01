#include <stdio.h>
#include <stdlib.h>

#include "simson.h"

#ifndef SIMSON_TEST_DATA_DIR
#define SIMSON_TEST_DATA_DIR "tests/data"
#endif

char* read_file(const char* filename, size_t* len) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    size_t fsz = ftell(f);
    rewind(f);
    char* buffer = malloc(fsz);
    if (!buffer) {
        fclose(f);
        return NULL;
    }
    size_t read = fread(buffer, 1, fsz, f);
    if (len) *len = read;
    fclose(f);
    return buffer;
}

int parse_y_file(const char* filename) {
    size_t len = 0;
    char* stream = read_file(filename, &len);
    if (!stream) return 1;
    simson_value_t* v = simson_value_parse(stream, len);
    free(stream);
    if (!v) return 1;
    simson_value_destroy(v);
    return 0;
}

int parse_n_file(const char* filename) {
    size_t len = 0;
    char* stream = read_file(filename, &len);
    if (!stream) return 1;
    simson_value_t* v = simson_value_parse(stream, len);
    free(stream);
    if (v) {
        simson_value_destroy(v);
        return 1;
    }
    return 0;
}

#define TEST_Y_FILE(filename) do {                  \
    printf("%s: ", filename);                   \
    if (parse_y_file((filename))) { printf("FAIL\n"); return 1; } \
    else     { printf("OK\n"); }             \
} while (0)

#define TEST_N_FILE(filename) do {                  \
    printf("%s: ", filename);                   \
    if (parse_n_file((filename))) { printf("FAIL\n"); return 1; } \
    else     { printf("OK\n"); }             \
} while (0)

char* y_files[] = {
    SIMSON_TEST_DATA_DIR "/y_array_arraysWithSpaces.json",
    SIMSON_TEST_DATA_DIR "/y_array_empty-string.json",
    SIMSON_TEST_DATA_DIR "/y_array_empty.json",
    SIMSON_TEST_DATA_DIR "/y_array_ending_with_newline.json",
    SIMSON_TEST_DATA_DIR "/y_array_false.json",
    SIMSON_TEST_DATA_DIR "/y_array_heterogeneous.json",
    SIMSON_TEST_DATA_DIR "/y_array_null.json",
    SIMSON_TEST_DATA_DIR "/y_array_with_1_and_newline.json",
    SIMSON_TEST_DATA_DIR "/y_array_with_leading_space.json",
    SIMSON_TEST_DATA_DIR "/y_array_with_several_null.json",
    SIMSON_TEST_DATA_DIR "/y_array_with_trailing_space.json",
    SIMSON_TEST_DATA_DIR "/y_number_0e+1.json",
    SIMSON_TEST_DATA_DIR "/y_number_0e1.json",
    SIMSON_TEST_DATA_DIR "/y_number_after_space.json",
    SIMSON_TEST_DATA_DIR "/y_number_double_close_to_zero.json",
    SIMSON_TEST_DATA_DIR "/y_number_int_with_exp.json",
    SIMSON_TEST_DATA_DIR "/y_number_minus_zero.json",
    SIMSON_TEST_DATA_DIR "/y_number_negative_int.json",
    SIMSON_TEST_DATA_DIR "/y_number_negative_one.json",
    SIMSON_TEST_DATA_DIR "/y_number_negative_zero.json",
    SIMSON_TEST_DATA_DIR "/y_number_real_capital_e_neg_exp.json",
    SIMSON_TEST_DATA_DIR "/y_number_real_capital_e_pos_exp.json",
    SIMSON_TEST_DATA_DIR "/y_number_real_capital_e.json",
    SIMSON_TEST_DATA_DIR "/y_number_real_exponent.json",
    SIMSON_TEST_DATA_DIR "/y_number_real_fraction_exponent.json",
    SIMSON_TEST_DATA_DIR "/y_number_real_neg_exp.json",
    SIMSON_TEST_DATA_DIR "/y_number_real_pos_exponent.json",
    SIMSON_TEST_DATA_DIR "/y_number_simple_int.json",
    SIMSON_TEST_DATA_DIR "/y_number_simple_real.json",
    SIMSON_TEST_DATA_DIR "/y_number.json",
    SIMSON_TEST_DATA_DIR "/y_object_basic.json",
    SIMSON_TEST_DATA_DIR "/y_object_duplicated_key_and_value.json",
    SIMSON_TEST_DATA_DIR "/y_object_duplicated_key.json",
    SIMSON_TEST_DATA_DIR "/y_object_empty_key.json",
    SIMSON_TEST_DATA_DIR "/y_object_empty.json",
    SIMSON_TEST_DATA_DIR "/y_object_escaped_null_in_key.json",
    SIMSON_TEST_DATA_DIR "/y_object_extreme_numbers.json",
    SIMSON_TEST_DATA_DIR "/y_object_long_strings.json",
    SIMSON_TEST_DATA_DIR "/y_object_simple.json",
    SIMSON_TEST_DATA_DIR "/y_object_string_unicode.json",
    SIMSON_TEST_DATA_DIR "/y_object_with_newlines.json",
    SIMSON_TEST_DATA_DIR "/y_object.json",
    SIMSON_TEST_DATA_DIR "/y_string_1_2_3_bytes_UTF-8_sequences.json",
    SIMSON_TEST_DATA_DIR "/y_string_accepted_surrogate_pair.json",
    SIMSON_TEST_DATA_DIR "/y_string_accepted_surrogate_pairs.json",
    SIMSON_TEST_DATA_DIR "/y_string_allowed_escapes.json",
    SIMSON_TEST_DATA_DIR "/y_string_backslash_and_u_escaped_zero.json",
    SIMSON_TEST_DATA_DIR "/y_string_backslash_doublequotes.json",
    SIMSON_TEST_DATA_DIR "/y_string_comments.json",
    SIMSON_TEST_DATA_DIR "/y_string_double_escape_a.json",
    SIMSON_TEST_DATA_DIR "/y_string_double_escape_n.json",
    SIMSON_TEST_DATA_DIR "/y_string_escaped_control_character.json",
    SIMSON_TEST_DATA_DIR "/y_string_escaped_noncharacter.json",
    SIMSON_TEST_DATA_DIR "/y_string_in_array_with_leading_space.json",
    SIMSON_TEST_DATA_DIR "/y_string_in_array.json",
    SIMSON_TEST_DATA_DIR "/y_string_last_surrogates_1_and_2.json",
    SIMSON_TEST_DATA_DIR "/y_string_nbsp_uescaped.json",
    SIMSON_TEST_DATA_DIR "/y_string_nonCharacterInUTF-8_U+10FFFF.json",
    SIMSON_TEST_DATA_DIR "/y_string_nonCharacterInUTF-8_U+FFFF.json",
    SIMSON_TEST_DATA_DIR "/y_string_null_escape.json",
    SIMSON_TEST_DATA_DIR "/y_string_one-byte-utf-8.json",
    SIMSON_TEST_DATA_DIR "/y_string_pi.json",
    SIMSON_TEST_DATA_DIR "/y_string_reservedCharacterInUTF-8_U+1BFFF.json",
    SIMSON_TEST_DATA_DIR "/y_string_simple_ascii.json",
    SIMSON_TEST_DATA_DIR "/y_string_space.json",
    SIMSON_TEST_DATA_DIR "/y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",
    SIMSON_TEST_DATA_DIR "/y_string_three-byte-utf-8.json",
    SIMSON_TEST_DATA_DIR "/y_string_two-byte-utf-8.json",
    SIMSON_TEST_DATA_DIR "/y_string_u+2028_line_sep.json",
    SIMSON_TEST_DATA_DIR "/y_string_u+2029_par_sep.json",
    SIMSON_TEST_DATA_DIR "/y_string_uEscape.json",
    SIMSON_TEST_DATA_DIR "/y_string_uescaped_newline.json",
    SIMSON_TEST_DATA_DIR "/y_string_unescaped_char_delete.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode_2.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode_escaped_double_quote.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode_U+1FFFE_nonchar.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode_U+10FFFE_nonchar.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode_U+2064_invisible_plus.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode_U+FDD0_nonchar.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode_U+FFFE_nonchar.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicode.json",
    SIMSON_TEST_DATA_DIR "/y_string_unicodeEscapedBackslash.json",
    SIMSON_TEST_DATA_DIR "/y_string_utf8.json",
    SIMSON_TEST_DATA_DIR "/y_string_with_del_character.json",
    SIMSON_TEST_DATA_DIR "/y_structure_lonely_false.json",
    SIMSON_TEST_DATA_DIR "/y_structure_lonely_int.json",
    SIMSON_TEST_DATA_DIR "/y_structure_lonely_negative_real.json",
    SIMSON_TEST_DATA_DIR "/y_structure_lonely_null.json",
    SIMSON_TEST_DATA_DIR "/y_structure_lonely_string.json",
    SIMSON_TEST_DATA_DIR "/y_structure_lonely_true.json",
    SIMSON_TEST_DATA_DIR "/y_structure_string_empty.json",
    SIMSON_TEST_DATA_DIR "/y_structure_trailing_newline.json",
    SIMSON_TEST_DATA_DIR "/y_structure_true_in_array.json",
    SIMSON_TEST_DATA_DIR "/y_structure_whitespace_array.json"
};

char* n_files[] = {
    SIMSON_TEST_DATA_DIR "/n_array_1_true_without_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_a_invalid_utf8.json",
    SIMSON_TEST_DATA_DIR "/n_array_colon_instead_of_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_comma_after_close.json",
    SIMSON_TEST_DATA_DIR "/n_array_comma_and_number.json",
    SIMSON_TEST_DATA_DIR "/n_array_double_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_double_extra_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_extra_close.json",
    SIMSON_TEST_DATA_DIR "/n_array_extra_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_incomplete_invalid_value.json",
    SIMSON_TEST_DATA_DIR "/n_array_incomplete.json",
    SIMSON_TEST_DATA_DIR "/n_array_inner_array_no_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_invalid_utf8.json",
    SIMSON_TEST_DATA_DIR "/n_array_items_separated_by_semicolon.json",
    SIMSON_TEST_DATA_DIR "/n_array_just_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_just_minus.json",
    SIMSON_TEST_DATA_DIR "/n_array_missing_value.json",
    SIMSON_TEST_DATA_DIR "/n_array_newlines_unclosed.json",
    SIMSON_TEST_DATA_DIR "/n_array_number_and_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_number_and_several_commas.json",
    SIMSON_TEST_DATA_DIR "/n_array_spaces_vertical_tab_formfeed.json",
    SIMSON_TEST_DATA_DIR "/n_array_star_inside.json",
    SIMSON_TEST_DATA_DIR "/n_array_unclosed_trailing_comma.json",
    SIMSON_TEST_DATA_DIR "/n_array_unclosed_with_new_lines.json",
    SIMSON_TEST_DATA_DIR "/n_array_unclosed_with_object_inside.json",
    SIMSON_TEST_DATA_DIR "/n_array_unclosed.json",
    SIMSON_TEST_DATA_DIR "/n_incomplete_false.json",
    SIMSON_TEST_DATA_DIR "/n_incomplete_null.json",
    SIMSON_TEST_DATA_DIR "/n_incomplete_true.json",
    SIMSON_TEST_DATA_DIR "/n_multidigit_number_then_00.json",
    SIMSON_TEST_DATA_DIR "/n_number_-1.0..json",
    SIMSON_TEST_DATA_DIR "/n_number_-01.json",
    SIMSON_TEST_DATA_DIR "/n_number_-2..json",
    SIMSON_TEST_DATA_DIR "/n_number_-NaN.json",
    SIMSON_TEST_DATA_DIR "/n_number_.-1.json",
    SIMSON_TEST_DATA_DIR "/n_number_.2e-3.json",
    SIMSON_TEST_DATA_DIR "/n_number_++.json",
    SIMSON_TEST_DATA_DIR "/n_number_+1.json",
    SIMSON_TEST_DATA_DIR "/n_number_+Inf.json",
    SIMSON_TEST_DATA_DIR "/n_number_0_capital_E.json",
    SIMSON_TEST_DATA_DIR "/n_number_0_capital_E+.json",
    SIMSON_TEST_DATA_DIR "/n_number_0.1.2.json",
    SIMSON_TEST_DATA_DIR "/n_number_0.3e.json",
    SIMSON_TEST_DATA_DIR "/n_number_0.3e+.json",
    SIMSON_TEST_DATA_DIR "/n_number_0.e1.json",
    SIMSON_TEST_DATA_DIR "/n_number_0e.json",
    SIMSON_TEST_DATA_DIR "/n_number_0e+.json",
    SIMSON_TEST_DATA_DIR "/n_number_1_000.json",
    SIMSON_TEST_DATA_DIR "/n_number_1.0e-.json",
    SIMSON_TEST_DATA_DIR "/n_number_1.0e.json",
    SIMSON_TEST_DATA_DIR "/n_number_1.0e+.json",
    SIMSON_TEST_DATA_DIR "/n_number_1eE2.json",
    SIMSON_TEST_DATA_DIR "/n_number_2.e-3.json",
    SIMSON_TEST_DATA_DIR "/n_number_2.e+3.json",
    SIMSON_TEST_DATA_DIR "/n_number_2.e3.json",
    SIMSON_TEST_DATA_DIR "/n_number_9.e+.json",
    SIMSON_TEST_DATA_DIR "/n_number_expression.json",
    SIMSON_TEST_DATA_DIR "/n_number_hex_1_digit.json",
    SIMSON_TEST_DATA_DIR "/n_number_hex_2_digits.json",
    SIMSON_TEST_DATA_DIR "/n_number_Inf.json",
    SIMSON_TEST_DATA_DIR "/n_number_infinity.json",
    SIMSON_TEST_DATA_DIR "/n_number_invalid-negative-real.json",
    SIMSON_TEST_DATA_DIR "/n_number_invalid-utf-8-in-bigger-int.json",
    SIMSON_TEST_DATA_DIR "/n_number_invalid-utf-8-in-exponent.json",
    SIMSON_TEST_DATA_DIR "/n_number_invalid-utf-8-in-int.json",
    SIMSON_TEST_DATA_DIR "/n_number_invalid+-.json",
    SIMSON_TEST_DATA_DIR "/n_number_minus_infinity.json",
    SIMSON_TEST_DATA_DIR "/n_number_minus_sign_with_trailing_garbage.json",
    SIMSON_TEST_DATA_DIR "/n_number_minus_space_1.json",
    SIMSON_TEST_DATA_DIR "/n_number_NaN.json",
    SIMSON_TEST_DATA_DIR "/n_number_neg_int_starting_with_zero.json",
    SIMSON_TEST_DATA_DIR "/n_number_neg_real_without_int_part.json",
    SIMSON_TEST_DATA_DIR "/n_number_neg_with_garbage_at_end.json",
    SIMSON_TEST_DATA_DIR "/n_number_real_garbage_after_e.json",
    SIMSON_TEST_DATA_DIR "/n_number_real_with_invalid_utf8_after_e.json",
    SIMSON_TEST_DATA_DIR "/n_number_real_without_fractional_part.json",
    SIMSON_TEST_DATA_DIR "/n_number_starting_with_dot.json",
    SIMSON_TEST_DATA_DIR "/n_number_U+FF11_fullwidth_digit_one.json",
    SIMSON_TEST_DATA_DIR "/n_number_with_alpha_char.json",
    SIMSON_TEST_DATA_DIR "/n_number_with_alpha.json",
    SIMSON_TEST_DATA_DIR "/n_number_with_leading_zero.json",
    SIMSON_TEST_DATA_DIR "/n_object_bad_value.json",
    SIMSON_TEST_DATA_DIR "/n_object_bracket_key.json",
    SIMSON_TEST_DATA_DIR "/n_object_comma_instead_of_colon.json",
    SIMSON_TEST_DATA_DIR "/n_object_double_colon.json",
    SIMSON_TEST_DATA_DIR "/n_object_emoji.json",
    SIMSON_TEST_DATA_DIR "/n_object_garbage_at_end.json",
    SIMSON_TEST_DATA_DIR "/n_object_key_with_single_quotes.json",
    SIMSON_TEST_DATA_DIR "/n_object_lone_continuation_byte_in_key_and_trailing_comma.json",
    SIMSON_TEST_DATA_DIR "/n_object_missing_colon.json",
    SIMSON_TEST_DATA_DIR "/n_object_missing_key.json",
    SIMSON_TEST_DATA_DIR "/n_object_missing_semicolon.json",
    SIMSON_TEST_DATA_DIR "/n_object_missing_value.json",
    SIMSON_TEST_DATA_DIR "/n_object_no-colon.json",
    SIMSON_TEST_DATA_DIR "/n_object_non_string_key_but_huge_number_instead.json",
    SIMSON_TEST_DATA_DIR "/n_object_non_string_key.json",
    SIMSON_TEST_DATA_DIR "/n_object_repeated_null_null.json",
    SIMSON_TEST_DATA_DIR "/n_object_several_trailing_commas.json",
    SIMSON_TEST_DATA_DIR "/n_object_single_quote.json",
    SIMSON_TEST_DATA_DIR "/n_object_trailing_comma.json",
    SIMSON_TEST_DATA_DIR "/n_object_trailing_comment_open.json",
    SIMSON_TEST_DATA_DIR "/n_object_trailing_comment_slash_open_incomplete.json",
    SIMSON_TEST_DATA_DIR "/n_object_trailing_comment_slash_open.json",
    SIMSON_TEST_DATA_DIR "/n_object_trailing_comment.json",
    SIMSON_TEST_DATA_DIR "/n_object_two_commas_in_a_row.json",
    SIMSON_TEST_DATA_DIR "/n_object_unquoted_key.json",
    SIMSON_TEST_DATA_DIR "/n_object_unterminated-value.json",
    SIMSON_TEST_DATA_DIR "/n_object_with_single_string.json",
    SIMSON_TEST_DATA_DIR "/n_object_with_trailing_garbage.json",
    SIMSON_TEST_DATA_DIR "/n_single_space.json",
    SIMSON_TEST_DATA_DIR "/n_string_1_surrogate_then_escape_u.json",
    SIMSON_TEST_DATA_DIR "/n_string_1_surrogate_then_escape_u1.json",
    SIMSON_TEST_DATA_DIR "/n_string_1_surrogate_then_escape_u1x.json",
    SIMSON_TEST_DATA_DIR "/n_string_1_surrogate_then_escape.json",
    SIMSON_TEST_DATA_DIR "/n_string_accentuated_char_no_quotes.json",
    SIMSON_TEST_DATA_DIR "/n_string_backslash_00.json",
    SIMSON_TEST_DATA_DIR "/n_string_escape_x.json",
    SIMSON_TEST_DATA_DIR "/n_string_escaped_backslash_bad.json",
    SIMSON_TEST_DATA_DIR "/n_string_escaped_ctrl_char_tab.json",
    SIMSON_TEST_DATA_DIR "/n_string_escaped_emoji.json",
    SIMSON_TEST_DATA_DIR "/n_string_incomplete_escape.json",
    SIMSON_TEST_DATA_DIR "/n_string_incomplete_escaped_character.json",
    SIMSON_TEST_DATA_DIR "/n_string_incomplete_surrogate_escape_invalid.json",
    SIMSON_TEST_DATA_DIR "/n_string_incomplete_surrogate.json",
    SIMSON_TEST_DATA_DIR "/n_string_invalid_backslash_esc.json",
    SIMSON_TEST_DATA_DIR "/n_string_invalid_unicode_escape.json",
    SIMSON_TEST_DATA_DIR "/n_string_invalid_utf8_after_escape.json",
    SIMSON_TEST_DATA_DIR "/n_string_invalid-utf-8-in-escape.json",
    SIMSON_TEST_DATA_DIR "/n_string_leading_uescaped_thinspace.json",
    SIMSON_TEST_DATA_DIR "/n_string_no_quotes_with_bad_escape.json",
    SIMSON_TEST_DATA_DIR "/n_string_single_doublequote.json",
    SIMSON_TEST_DATA_DIR "/n_string_single_quote.json",
    SIMSON_TEST_DATA_DIR "/n_string_single_string_no_double_quotes.json",
    SIMSON_TEST_DATA_DIR "/n_string_start_escape_unclosed.json",
    SIMSON_TEST_DATA_DIR "/n_string_unescaped_ctrl_char.json",
    SIMSON_TEST_DATA_DIR "/n_string_unescaped_newline.json",
    SIMSON_TEST_DATA_DIR "/n_string_unescaped_tab.json",
    SIMSON_TEST_DATA_DIR "/n_string_unicode_CapitalU.json",
    SIMSON_TEST_DATA_DIR "/n_string_with_trailing_garbage.json",
    SIMSON_TEST_DATA_DIR "/n_structure_100000_opening_arrays.json",
    SIMSON_TEST_DATA_DIR "/n_structure_angle_bracket_..json",
    SIMSON_TEST_DATA_DIR "/n_structure_angle_bracket_null.json",
    SIMSON_TEST_DATA_DIR "/n_structure_array_trailing_garbage.json",
    SIMSON_TEST_DATA_DIR "/n_structure_array_with_extra_array_close.json",
    SIMSON_TEST_DATA_DIR "/n_structure_array_with_unclosed_string.json",
    SIMSON_TEST_DATA_DIR "/n_structure_ascii-unicode-identifier.json",
    SIMSON_TEST_DATA_DIR "/n_structure_capitalized_True.json",
    SIMSON_TEST_DATA_DIR "/n_structure_close_unopened_array.json",
    SIMSON_TEST_DATA_DIR "/n_structure_comma_instead_of_closing_brace.json",
    SIMSON_TEST_DATA_DIR "/n_structure_double_array.json",
    SIMSON_TEST_DATA_DIR "/n_structure_end_array.json",
    SIMSON_TEST_DATA_DIR "/n_structure_incomplete_UTF8_BOM.json",
    SIMSON_TEST_DATA_DIR "/n_structure_lone-invalid-utf-8.json",
    SIMSON_TEST_DATA_DIR "/n_structure_lone-open-bracket.json",
    SIMSON_TEST_DATA_DIR "/n_structure_no_data.json",
    SIMSON_TEST_DATA_DIR "/n_structure_null-byte-outside-string.json",
    SIMSON_TEST_DATA_DIR "/n_structure_number_with_trailing_garbage.json",
    SIMSON_TEST_DATA_DIR "/n_structure_object_followed_by_closing_object.json",
    SIMSON_TEST_DATA_DIR "/n_structure_object_unclosed_no_value.json",
    SIMSON_TEST_DATA_DIR "/n_structure_object_with_comment.json",
    SIMSON_TEST_DATA_DIR "/n_structure_object_with_trailing_garbage.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_array_apostrophe.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_array_comma.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_array_object.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_array_open_object.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_array_open_string.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_array_string.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_object_close_array.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_object_comma.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_object_open_array.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_object_open_string.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_object_string_with_apostrophes.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_object.json",
    SIMSON_TEST_DATA_DIR "/n_structure_open_open.json",
    SIMSON_TEST_DATA_DIR "/n_structure_single_eacute.json",
    SIMSON_TEST_DATA_DIR "/n_structure_single_star.json",
    SIMSON_TEST_DATA_DIR "/n_structure_trailing_#.json",
    SIMSON_TEST_DATA_DIR "/n_structure_U+2060_word_joined.json",
    SIMSON_TEST_DATA_DIR "/n_structure_uescaped_LF_before_string.json",
    SIMSON_TEST_DATA_DIR "/n_structure_unclosed_array_partial_null.json",
    SIMSON_TEST_DATA_DIR "/n_structure_unclosed_array_unfinished_false.json",
    SIMSON_TEST_DATA_DIR "/n_structure_unclosed_array_unfinished_true.json",
    SIMSON_TEST_DATA_DIR "/n_structure_unclosed_array.json",
    SIMSON_TEST_DATA_DIR "/n_structure_unclosed_object.json",
    SIMSON_TEST_DATA_DIR "/n_structure_unicode-identifier.json",
    SIMSON_TEST_DATA_DIR "/n_structure_UTF8_BOM_no_data.json",
    SIMSON_TEST_DATA_DIR "/n_structure_whitespace_formfeed.json",
    SIMSON_TEST_DATA_DIR "/n_structure_whitespace_U+2060_word_joiner.json"
};

int main(void) {
    for (size_t i = 0; i < sizeof(y_files) / (sizeof(char*)); i++) {
        TEST_Y_FILE(y_files[i]);
    }
    for (size_t i = 0; i < sizeof(n_files) / (sizeof(char*)); i++) {
         TEST_N_FILE(n_files[i]);
    }
    return 0;
}
