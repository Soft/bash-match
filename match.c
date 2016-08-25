/*
bash-match - Enhanced regular expression matching and capturing for Bash.
Copyright (C) 2016  Samuel Laurén

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE 1

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include <pcre.h>

#include <bashtypes.h>
#include <builtins.h>
#include <shell.h>
#include <common.h>

#define MAX_GROUP_COUNT 30
#define MATCH_SHORT_DOC "match pattern string [name ...]"


static int match_function(WORD_LIST *list) {
	char *pattern, *subject, *group;
	const char *error_string;
	pcre *compiled;
	int error_offset,
		result,
		num_results,
		len,
		ret = EXECUTION_SUCCESS,
		vector[MAX_GROUP_COUNT];
	SHELL_VAR *var;

	if (list == NULL || list->next == NULL) {
		builtin_usage();
		return EX_USAGE;
	}

	pattern = list->word->word;
	subject = list->next->word->word;
	list = list->next->next;
	len = strlen(subject);

	compiled = pcre_compile(pattern, PCRE_UTF8, &error_string, &error_offset, NULL);
	if (compiled == NULL) {
		builtin_error("invalid regular expression: %s at the position %d", error_string, error_offset);
		return EXECUTION_FAILURE;
	}

	result = pcre_exec(compiled, NULL, subject, len, 0, 0, vector, MAX_GROUP_COUNT);
	if (result == PCRE_ERROR_NOMATCH) { // FIXME: Other errors
		ret = EXIT_FAILURE;
		goto done;
	}

	if (list == NULL) {
		goto done;
	}

	num_results = result > 0 ? result : MAX_GROUP_COUNT / 3;

	var = find_variable(list->word->word);
	if (var != 0 && array_p(var) != 0) {
		for (int i = 0; i < num_results; i++) {
			pcre_get_substring(subject, vector, num_results, i, (const char **)&group);
			array_insert(array_cell(var), i, group);
			pcre_free_substring(group);
		}
	} else {
		for (int i = 0; list && i < num_results; list = list->next, i++) {
			if (legal_identifier(list->word->word) == 0) {
				sh_invalidid(list->word->word);
				ret = EXECUTION_FAILURE;
				goto done;
			}
			pcre_get_substring(subject, vector, num_results, i, (const char **)&group);
			bind_variable(list->word->word, group, 0);
			pcre_free_substring(group);
		}
	}

 done:
	pcre_free(compiled);
	return ret;
}


static char *match_long_doc[] = {
	"Match strings using regular expressions",
	"",
	"Matches STRING against PATTERN and stores captures in NAMEs.",
	"If the first NAME has been declared as an array, it will be used to store",
	"the captures.",
	"",
	"Exit Status:",
	"The return code is zero if the match was succesful. If the string does not",
	"match the pattern, a return code of one will be returned.",
	"",
	NULL
};


struct builtin match_struct = {
	.name = "match",
	.function = match_function,
	.flags = BUILTIN_ENABLED,
	.long_doc = match_long_doc,
	.short_doc = MATCH_SHORT_DOC,
	.handle = 0
};


