# Read the file and count the number of rows in each section
file_path = "up_down3.txt"

sections = []
current_section = []

with open(file_path, "r") as file:
    for line in file:
        line = line.strip()
        if line == "":  # Empty line indicates a new section
            if current_section:
                sections.append(len(current_section))
                current_section = []
        else:
            current_section.append(line)

# Add the last section if the file didn't end with an empty line
if current_section:
    sections.append(len(current_section))

# Print section row counts
section_counts = {f"Section {i}": count for i, count in enumerate(sections)}

for key, value in section_counts.items():
    print(f"{key}: {value}")
