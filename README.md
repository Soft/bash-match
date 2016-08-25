# Matching for Bash

`bash-match` is an experimental loadable for
[Bash](https://www.gnu.org/software/bash/), that aims to make working with text
a joy. It enhances Bash with a single new built-in `match`.

	match pattern string [name ...]

## Usage

`match` can check if a string matches a pattern:

	match 'Hello World' 'Hello World'
	echo $?
	=> 0
	
	match 'Foo' 'Bar'
	echo $?
	=> 1
	
	match '^(allow|deny) \d{1,3}$' 'allow 42'
	echo $?
	=> 0
	
This can be, for example, combined with the `if` construct, but admittedly
offers little over the natively supported `=~` comparison operator. Where
`match` shines is capturing:

	match 'entry-(\d+)' 'foo entry-42 bar' full id
	echo $?
	=> 0
	echo "$full"
	=> entry-42
	echo "$id"
	=> 42
	
`match` assigns the results of captures to the variables passed to it. The first
variable will contain the entire match.

	declare -a results
	match '\d{3}-\d{3}-\d{3}' '100-200-300' results
	echo "${results[0]}"
	=> 100-200-300
	echo "${results[1]}"
	=> 100
	echo "${results[2]}"
	=> 200
	echo "${results[3]}"
	=> 300

If the variable has been previously declared as an array, it will be used to
store the captures.
	
## Installation

## Copying

`bash-match` is licensed under the GNU General Public License Version 3.

