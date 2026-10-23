#include <stdio.h>
#include <stdlib.h>

#include "simson.h"

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
    "/home/tony/dev/simson/tests/data/y_array_arraysWithSpaces.json",
    "/home/tony/dev/simson/tests/data/y_array_empty-string.json",
    "/home/tony/dev/simson/tests/data/y_array_empty.json",
    "/home/tony/dev/simson/tests/data/y_array_ending_with_newline.json",
    "/home/tony/dev/simson/tests/data/y_array_false.json",
    "/home/tony/dev/simson/tests/data/y_array_heterogeneous.json",
    "/home/tony/dev/simson/tests/data/y_array_null.json",
    "/home/tony/dev/simson/tests/data/y_array_with_1_and_newline.json",
    "/home/tony/dev/simson/tests/data/y_array_with_leading_space.json",
    "/home/tony/dev/simson/tests/data/y_array_with_several_null.json",
    "/home/tony/dev/simson/tests/data/y_array_with_trailing_space.json",
    "/home/tony/dev/simson/tests/data/y_number_0e+1.json",
    "/home/tony/dev/simson/tests/data/y_number_0e1.json",
    "/home/tony/dev/simson/tests/data/y_number_after_space.json",
    "/home/tony/dev/simson/tests/data/y_number_double_close_to_zero.json",
    "/home/tony/dev/simson/tests/data/y_number_int_with_exp.json",
    "/home/tony/dev/simson/tests/data/y_number_minus_zero.json",
    "/home/tony/dev/simson/tests/data/y_number_negative_int.json",
    "/home/tony/dev/simson/tests/data/y_number_negative_one.json",
    "/home/tony/dev/simson/tests/data/y_number_negative_zero.json",
    "/home/tony/dev/simson/tests/data/y_number_real_capital_e_neg_exp.json",
    "/home/tony/dev/simson/tests/data/y_number_real_capital_e_pos_exp.json",
    "/home/tony/dev/simson/tests/data/y_number_real_capital_e.json",
    "/home/tony/dev/simson/tests/data/y_number_real_exponent.json",
    "/home/tony/dev/simson/tests/data/y_number_real_fraction_exponent.json",
    "/home/tony/dev/simson/tests/data/y_number_real_neg_exp.json",
    "/home/tony/dev/simson/tests/data/y_number_real_pos_exponent.json",
    "/home/tony/dev/simson/tests/data/y_number_simple_int.json",
    "/home/tony/dev/simson/tests/data/y_number_simple_real.json",
    "/home/tony/dev/simson/tests/data/y_number.json",
    "/home/tony/dev/simson/tests/data/y_object_basic.json",
    "/home/tony/dev/simson/tests/data/y_object_duplicated_key_and_value.json",
    "/home/tony/dev/simson/tests/data/y_object_duplicated_key.json",
    "/home/tony/dev/simson/tests/data/y_object_empty_key.json",
    "/home/tony/dev/simson/tests/data/y_object_empty.json",
    "/home/tony/dev/simson/tests/data/y_object_escaped_null_in_key.json",
    "/home/tony/dev/simson/tests/data/y_object_extreme_numbers.json",
    "/home/tony/dev/simson/tests/data/y_object_long_strings.json",
    "/home/tony/dev/simson/tests/data/y_object_simple.json",
    "/home/tony/dev/simson/tests/data/y_object_string_unicode.json",
    "/home/tony/dev/simson/tests/data/y_object_with_newlines.json",
    "/home/tony/dev/simson/tests/data/y_object.json",
    "/home/tony/dev/simson/tests/data/y_string_1_2_3_bytes_UTF-8_sequences.json",
    "/home/tony/dev/simson/tests/data/y_string_accepted_surrogate_pair.json",
    "/home/tony/dev/simson/tests/data/y_string_accepted_surrogate_pairs.json",
    "/home/tony/dev/simson/tests/data/y_string_allowed_escapes.json",
    "/home/tony/dev/simson/tests/data/y_string_backslash_and_u_escaped_zero.json",
    "/home/tony/dev/simson/tests/data/y_string_backslash_doublequotes.json",
    "/home/tony/dev/simson/tests/data/y_string_comments.json",
    "/home/tony/dev/simson/tests/data/y_string_double_escape_a.json",
    "/home/tony/dev/simson/tests/data/y_string_double_escape_n.json",
    "/home/tony/dev/simson/tests/data/y_string_escaped_control_character.json",
    "/home/tony/dev/simson/tests/data/y_string_escaped_noncharacter.json",
    "/home/tony/dev/simson/tests/data/y_string_in_array_with_leading_space.json",
    "/home/tony/dev/simson/tests/data/y_string_in_array.json",
    "/home/tony/dev/simson/tests/data/y_string_last_surrogates_1_and_2.json",
    "/home/tony/dev/simson/tests/data/y_string_nbsp_uescaped.json",
    "/home/tony/dev/simson/tests/data/y_string_nonCharacterInUTF-8_U+10FFFF.json",
    "/home/tony/dev/simson/tests/data/y_string_nonCharacterInUTF-8_U+FFFF.json",
    "/home/tony/dev/simson/tests/data/y_string_null_escape.json",
    "/home/tony/dev/simson/tests/data/y_string_one-byte-utf-8.json",
    "/home/tony/dev/simson/tests/data/y_string_pi.json",
    "/home/tony/dev/simson/tests/data/y_string_reservedCharacterInUTF-8_U+1BFFF.json",
    "/home/tony/dev/simson/tests/data/y_string_simple_ascii.json",
    "/home/tony/dev/simson/tests/data/y_string_space.json",
    "/home/tony/dev/simson/tests/data/y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF.json",
    "/home/tony/dev/simson/tests/data/y_string_three-byte-utf-8.json",
    "/home/tony/dev/simson/tests/data/y_string_two-byte-utf-8.json",
    "/home/tony/dev/simson/tests/data/y_string_u+2028_line_sep.json",
    "/home/tony/dev/simson/tests/data/y_string_u+2029_par_sep.json",
    "/home/tony/dev/simson/tests/data/y_string_uEscape.json",
    "/home/tony/dev/simson/tests/data/y_string_uescaped_newline.json",
    "/home/tony/dev/simson/tests/data/y_string_unescaped_char_delete.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode_2.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode_escaped_double_quote.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode_U+1FFFE_nonchar.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode_U+10FFFE_nonchar.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode_U+200B_ZERO_WIDTH_SPACE.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode_U+2064_invisible_plus.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode_U+FDD0_nonchar.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode_U+FFFE_nonchar.json",
    "/home/tony/dev/simson/tests/data/y_string_unicode.json",
    "/home/tony/dev/simson/tests/data/y_string_unicodeEscapedBackslash.json",
    "/home/tony/dev/simson/tests/data/y_string_utf8.json",
    "/home/tony/dev/simson/tests/data/y_string_with_del_character.json",
    "/home/tony/dev/simson/tests/data/y_structure_lonely_false.json",
    "/home/tony/dev/simson/tests/data/y_structure_lonely_int.json",
    "/home/tony/dev/simson/tests/data/y_structure_lonely_negative_real.json",
    "/home/tony/dev/simson/tests/data/y_structure_lonely_null.json",
    "/home/tony/dev/simson/tests/data/y_structure_lonely_string.json",
    "/home/tony/dev/simson/tests/data/y_structure_lonely_true.json",
    "/home/tony/dev/simson/tests/data/y_structure_string_empty.json",
    "/home/tony/dev/simson/tests/data/y_structure_trailing_newline.json",
    "/home/tony/dev/simson/tests/data/y_structure_true_in_array.json",
    "/home/tony/dev/simson/tests/data/y_structure_whitespace_array.json"
};

char* n_files[] = {
    "/home/tony/dev/simson/tests/data/n_array_1_true_without_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_a_invalid_utf8.json",
    "/home/tony/dev/simson/tests/data/n_array_colon_instead_of_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_comma_after_close.json",
    "/home/tony/dev/simson/tests/data/n_array_comma_and_number.json",
    "/home/tony/dev/simson/tests/data/n_array_double_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_double_extra_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_extra_close.json",
    "/home/tony/dev/simson/tests/data/n_array_extra_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_incomplete_invalid_value.json",
    "/home/tony/dev/simson/tests/data/n_array_incomplete.json",
    "/home/tony/dev/simson/tests/data/n_array_inner_array_no_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_invalid_utf8.json",
    "/home/tony/dev/simson/tests/data/n_array_items_separated_by_semicolon.json",
    "/home/tony/dev/simson/tests/data/n_array_just_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_just_minus.json",
    "/home/tony/dev/simson/tests/data/n_array_missing_value.json",
    "/home/tony/dev/simson/tests/data/n_array_newlines_unclosed.json",
    "/home/tony/dev/simson/tests/data/n_array_number_and_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_number_and_several_commas.json",
    "/home/tony/dev/simson/tests/data/n_array_spaces_vertical_tab_formfeed.json",
    "/home/tony/dev/simson/tests/data/n_array_star_inside.json",
    "/home/tony/dev/simson/tests/data/n_array_unclosed_trailing_comma.json",
    "/home/tony/dev/simson/tests/data/n_array_unclosed_with_new_lines.json",
    "/home/tony/dev/simson/tests/data/n_array_unclosed_with_object_inside.json",
    "/home/tony/dev/simson/tests/data/n_array_unclosed.json",
    "/home/tony/dev/simson/tests/data/n_incomplete_false.json",
    "/home/tony/dev/simson/tests/data/n_incomplete_null.json",
    "/home/tony/dev/simson/tests/data/n_incomplete_true.json",
    "/home/tony/dev/simson/tests/data/n_multidigit_number_then_00.json",
    "/home/tony/dev/simson/tests/data/n_number_-1.0..json",
    "/home/tony/dev/simson/tests/data/n_number_-01.json",
    "/home/tony/dev/simson/tests/data/n_number_-2..json",
    "/home/tony/dev/simson/tests/data/n_number_-NaN.json",
    "/home/tony/dev/simson/tests/data/n_number_.-1.json",
    "/home/tony/dev/simson/tests/data/n_number_.2e-3.json",
    "/home/tony/dev/simson/tests/data/n_number_++.json",
    "/home/tony/dev/simson/tests/data/n_number_+1.json",
    "/home/tony/dev/simson/tests/data/n_number_+Inf.json",
    "/home/tony/dev/simson/tests/data/n_number_0_capital_E.json",
    "/home/tony/dev/simson/tests/data/n_number_0_capital_E+.json",
    "/home/tony/dev/simson/tests/data/n_number_0.1.2.json",
    "/home/tony/dev/simson/tests/data/n_number_0.3e.json",
    "/home/tony/dev/simson/tests/data/n_number_0.3e+.json",
    "/home/tony/dev/simson/tests/data/n_number_0.e1.json",
    "/home/tony/dev/simson/tests/data/n_number_0e.json",
    "/home/tony/dev/simson/tests/data/n_number_0e+.json",
    "/home/tony/dev/simson/tests/data/n_number_1_000.json",
    "/home/tony/dev/simson/tests/data/n_number_1.0e-.json",
    "/home/tony/dev/simson/tests/data/n_number_1.0e.json",
    "/home/tony/dev/simson/tests/data/n_number_1.0e+.json",
    "/home/tony/dev/simson/tests/data/n_number_1eE2.json",
    "/home/tony/dev/simson/tests/data/n_number_2.e-3.json",
    "/home/tony/dev/simson/tests/data/n_number_2.e+3.json",
    "/home/tony/dev/simson/tests/data/n_number_2.e3.json",
    "/home/tony/dev/simson/tests/data/n_number_9.e+.json",
    "/home/tony/dev/simson/tests/data/n_number_expression.json",
    "/home/tony/dev/simson/tests/data/n_number_hex_1_digit.json",
    "/home/tony/dev/simson/tests/data/n_number_hex_2_digits.json",
    "/home/tony/dev/simson/tests/data/n_number_Inf.json",
    "/home/tony/dev/simson/tests/data/n_number_infinity.json",
    "/home/tony/dev/simson/tests/data/n_number_invalid-negative-real.json",
    "/home/tony/dev/simson/tests/data/n_number_invalid-utf-8-in-bigger-int.json",
    "/home/tony/dev/simson/tests/data/n_number_invalid-utf-8-in-exponent.json",
    "/home/tony/dev/simson/tests/data/n_number_invalid-utf-8-in-int.json",
    "/home/tony/dev/simson/tests/data/n_number_invalid+-.json",
    "/home/tony/dev/simson/tests/data/n_number_minus_infinity.json",
    "/home/tony/dev/simson/tests/data/n_number_minus_sign_with_trailing_garbage.json",
    "/home/tony/dev/simson/tests/data/n_number_minus_space_1.json",
    "/home/tony/dev/simson/tests/data/n_number_NaN.json",
    "/home/tony/dev/simson/tests/data/n_number_neg_int_starting_with_zero.json",
    "/home/tony/dev/simson/tests/data/n_number_neg_real_without_int_part.json",
    "/home/tony/dev/simson/tests/data/n_number_neg_with_garbage_at_end.json",
    "/home/tony/dev/simson/tests/data/n_number_real_garbage_after_e.json",
    "/home/tony/dev/simson/tests/data/n_number_real_with_invalid_utf8_after_e.json",
    "/home/tony/dev/simson/tests/data/n_number_real_without_fractional_part.json",
    "/home/tony/dev/simson/tests/data/n_number_starting_with_dot.json",
    "/home/tony/dev/simson/tests/data/n_number_U+FF11_fullwidth_digit_one.json",
    "/home/tony/dev/simson/tests/data/n_number_with_alpha_char.json",
    "/home/tony/dev/simson/tests/data/n_number_with_alpha.json",
    "/home/tony/dev/simson/tests/data/n_number_with_leading_zero.json",
    "/home/tony/dev/simson/tests/data/n_object_bad_value.json",
    "/home/tony/dev/simson/tests/data/n_object_bracket_key.json",
    "/home/tony/dev/simson/tests/data/n_object_comma_instead_of_colon.json",
    "/home/tony/dev/simson/tests/data/n_object_double_colon.json",
    "/home/tony/dev/simson/tests/data/n_object_emoji.json",
    "/home/tony/dev/simson/tests/data/n_object_garbage_at_end.json",
    "/home/tony/dev/simson/tests/data/n_object_key_with_single_quotes.json",
    "/home/tony/dev/simson/tests/data/n_object_lone_continuation_byte_in_key_and_trailing_comma.json",
    "/home/tony/dev/simson/tests/data/n_object_missing_colon.json",
    "/home/tony/dev/simson/tests/data/n_object_missing_key.json",
    "/home/tony/dev/simson/tests/data/n_object_missing_semicolon.json",
    "/home/tony/dev/simson/tests/data/n_object_missing_value.json",
    "/home/tony/dev/simson/tests/data/n_object_no-colon.json",
    "/home/tony/dev/simson/tests/data/n_object_non_string_key_but_huge_number_instead.json",
    "/home/tony/dev/simson/tests/data/n_object_non_string_key.json",
    "/home/tony/dev/simson/tests/data/n_object_repeated_null_null.json",
    "/home/tony/dev/simson/tests/data/n_object_several_trailing_commas.json",
    "/home/tony/dev/simson/tests/data/n_object_single_quote.json",
    "/home/tony/dev/simson/tests/data/n_object_trailing_comma.json",
    "/home/tony/dev/simson/tests/data/n_object_trailing_comment_open.json",
    "/home/tony/dev/simson/tests/data/n_object_trailing_comment_slash_open_incomplete.json",
    "/home/tony/dev/simson/tests/data/n_object_trailing_comment_slash_open.json",
    "/home/tony/dev/simson/tests/data/n_object_trailing_comment.json",
    "/home/tony/dev/simson/tests/data/n_object_two_commas_in_a_row.json",
    "/home/tony/dev/simson/tests/data/n_object_unquoted_key.json",
    "/home/tony/dev/simson/tests/data/n_object_unterminated-value.json",
    "/home/tony/dev/simson/tests/data/n_object_with_single_string.json",
    "/home/tony/dev/simson/tests/data/n_object_with_trailing_garbage.json",
    "/home/tony/dev/simson/tests/data/n_single_space.json",
    "/home/tony/dev/simson/tests/data/n_string_1_surrogate_then_escape_u.json",
    "/home/tony/dev/simson/tests/data/n_string_1_surrogate_then_escape_u1.json",
    "/home/tony/dev/simson/tests/data/n_string_1_surrogate_then_escape_u1x.json",
    "/home/tony/dev/simson/tests/data/n_string_1_surrogate_then_escape.json",
    "/home/tony/dev/simson/tests/data/n_string_accentuated_char_no_quotes.json",
    "/home/tony/dev/simson/tests/data/n_string_backslash_00.json",
    "/home/tony/dev/simson/tests/data/n_string_escape_x.json",
    "/home/tony/dev/simson/tests/data/n_string_escaped_backslash_bad.json",
    "/home/tony/dev/simson/tests/data/n_string_escaped_ctrl_char_tab.json",
    "/home/tony/dev/simson/tests/data/n_string_escaped_emoji.json",
    "/home/tony/dev/simson/tests/data/n_string_incomplete_escape.json",
    "/home/tony/dev/simson/tests/data/n_string_incomplete_escaped_character.json",
    "/home/tony/dev/simson/tests/data/n_string_incomplete_surrogate_escape_invalid.json",
    "/home/tony/dev/simson/tests/data/n_string_incomplete_surrogate.json",
    "/home/tony/dev/simson/tests/data/n_string_invalid_backslash_esc.json",
    "/home/tony/dev/simson/tests/data/n_string_invalid_unicode_escape.json",
    "/home/tony/dev/simson/tests/data/n_string_invalid_utf8_after_escape.json",
    "/home/tony/dev/simson/tests/data/n_string_invalid-utf-8-in-escape.json",
    "/home/tony/dev/simson/tests/data/n_string_leading_uescaped_thinspace.json",
    "/home/tony/dev/simson/tests/data/n_string_no_quotes_with_bad_escape.json",
    "/home/tony/dev/simson/tests/data/n_string_single_doublequote.json",
    "/home/tony/dev/simson/tests/data/n_string_single_quote.json",
    "/home/tony/dev/simson/tests/data/n_string_single_string_no_double_quotes.json",
    "/home/tony/dev/simson/tests/data/n_string_start_escape_unclosed.json",
    "/home/tony/dev/simson/tests/data/n_string_unescaped_ctrl_char.json",
    "/home/tony/dev/simson/tests/data/n_string_unescaped_newline.json",
    "/home/tony/dev/simson/tests/data/n_string_unescaped_tab.json",
    "/home/tony/dev/simson/tests/data/n_string_unicode_CapitalU.json",
    "/home/tony/dev/simson/tests/data/n_string_with_trailing_garbage.json",
    "/home/tony/dev/simson/tests/data/n_structure_100000_opening_arrays.json",
    "/home/tony/dev/simson/tests/data/n_structure_angle_bracket_..json",
    "/home/tony/dev/simson/tests/data/n_structure_angle_bracket_null.json",
    "/home/tony/dev/simson/tests/data/n_structure_array_trailing_garbage.json",
    "/home/tony/dev/simson/tests/data/n_structure_array_with_extra_array_close.json",
    "/home/tony/dev/simson/tests/data/n_structure_array_with_unclosed_string.json",
    "/home/tony/dev/simson/tests/data/n_structure_ascii-unicode-identifier.json",
    "/home/tony/dev/simson/tests/data/n_structure_capitalized_True.json",
    "/home/tony/dev/simson/tests/data/n_structure_close_unopened_array.json",
    "/home/tony/dev/simson/tests/data/n_structure_comma_instead_of_closing_brace.json",
    "/home/tony/dev/simson/tests/data/n_structure_double_array.json",
    "/home/tony/dev/simson/tests/data/n_structure_end_array.json",
    "/home/tony/dev/simson/tests/data/n_structure_incomplete_UTF8_BOM.json",
    "/home/tony/dev/simson/tests/data/n_structure_lone-invalid-utf-8.json",
    "/home/tony/dev/simson/tests/data/n_structure_lone-open-bracket.json",
    "/home/tony/dev/simson/tests/data/n_structure_no_data.json",
    "/home/tony/dev/simson/tests/data/n_structure_null-byte-outside-string.json",
    "/home/tony/dev/simson/tests/data/n_structure_number_with_trailing_garbage.json",
    "/home/tony/dev/simson/tests/data/n_structure_object_followed_by_closing_object.json",
    "/home/tony/dev/simson/tests/data/n_structure_object_unclosed_no_value.json",
    "/home/tony/dev/simson/tests/data/n_structure_object_with_comment.json",
    "/home/tony/dev/simson/tests/data/n_structure_object_with_trailing_garbage.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_array_apostrophe.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_array_comma.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_array_object.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_array_open_object.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_array_open_string.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_array_string.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_object_close_array.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_object_comma.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_object_open_array.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_object_open_string.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_object_string_with_apostrophes.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_object.json",
    "/home/tony/dev/simson/tests/data/n_structure_open_open.json",
    "/home/tony/dev/simson/tests/data/n_structure_single_eacute.json",
    "/home/tony/dev/simson/tests/data/n_structure_single_star.json",
    "/home/tony/dev/simson/tests/data/n_structure_trailing_#.json",
    "/home/tony/dev/simson/tests/data/n_structure_U+2060_word_joined.json",
    "/home/tony/dev/simson/tests/data/n_structure_uescaped_LF_before_string.json",
    "/home/tony/dev/simson/tests/data/n_structure_unclosed_array_partial_null.json",
    "/home/tony/dev/simson/tests/data/n_structure_unclosed_array_unfinished_false.json",
    "/home/tony/dev/simson/tests/data/n_structure_unclosed_array_unfinished_true.json",
    "/home/tony/dev/simson/tests/data/n_structure_unclosed_array.json",
    "/home/tony/dev/simson/tests/data/n_structure_unclosed_object.json",
    "/home/tony/dev/simson/tests/data/n_structure_unicode-identifier.json",
    "/home/tony/dev/simson/tests/data/n_structure_UTF8_BOM_no_data.json",
    "/home/tony/dev/simson/tests/data/n_structure_whitespace_formfeed.json",
    "/home/tony/dev/simson/tests/data/n_structure_whitespace_U+2060_word_joiner.json"
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