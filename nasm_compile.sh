#!/bin/bash

# How to use:
# bash nasm_compile.sh 'file name w/o the extension' -exec [optional, to execute the program]
# Example:
# bash nasm_compile.sh hello_world -exec ---create the executable and execute it imediately
# bash nasm_compile.sh hello_world ---just create the executable

function createOutputFile {
	printf "Creating output file...\n"
	nasm -felf64 $ASMPATH$1.asm -o $ASM_PATH$1.o
	if [[ $? == 0 ]]; then
		createExecutable $1
	fi
}

function createExecutable {
	printf "Creating executable...\n"
	ld -o $ASM_PATH$1 $ASM_PATH$1.o
	if [[ $? == 0 ]]; then
		turnToExecutable $1
	fi
}

function turnToExecutable {
	printf "Configuring executable...\n"
	chmod u+x $ASM_PATH$1
	if [[ $? == 0 ]]; then
		printf "Done! '%s' created successfuly!\n" $1
	fi
}

function execute {
	if [[ $2 == "-exec" ]]; then
		./$1
	fi
}


# main() function
if [[ $1 != "" ]]; then # the first argument is required
	if [[ $1 != *".asm"* ]]; then # the first argument can't have the extension .asm
		createOutputFile $1
	fi
fi

execute $1 $2
