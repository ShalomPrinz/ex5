END_INPUT_CHAR = '4'
MENU_OUTPUT = """Please Choose:
	1. Watch playlists
	2. Add playlist
	3. Remove playlist
	4. exit
Goodbye!"""

def process_test_file(filename):
    with open(filename, 'r') as file:
        content = file.read()

    tests = content.split('\n- name:')
    if tests[0] == '': tests.pop(0)

    combined_input = ""
    combined_output = ""

    for test in tests:
        # Handle input
        input_start = test.find('input: |') + 8
        input_end = test.find('  output:')
        test_input = test[input_start:input_end].strip()

        # Handle output
        output_start = test.find('output: |') + 9
        output_end = test.find('\n- name:', output_start) if test.find('\n- name:', output_start) != -1 else len(test)
        test_output = test[output_start:output_end].strip()

        # Process input
        if END_INPUT_CHAR in test_input:
            test_input = test_input[:test_input.rfind(END_INPUT_CHAR)]
        combined_input += test_input.strip() + '\n'

        # Process output
        if MENU_OUTPUT in test_output:
            test_output = test_output[:test_output.find(MENU_OUTPUT)].strip()
        combined_output += test_output + '\n'

    new_test = f"""- name: One pass test
  input: |{combined_input.rstrip()}
{END_INPUT_CHAR}
  output: |{combined_output.rstrip()}
{MENU_OUTPUT}
"""

    with open('merged_test.txt', 'w') as file:
        file.write(new_test)

process_test_file('test_cases.txt')