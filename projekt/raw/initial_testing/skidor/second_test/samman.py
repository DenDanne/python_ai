import os

# Ange sökvägen där mapparna ligger
root_folder = r"C:\Users\danne\OneDrive\Skrivbord\Universitetet\nti\daniel_l_git\projekt\raw\initial_testing\skidor\second_test"

# Loopa igenom varje undermapp i root_folder
for folder in os.listdir(root_folder):
    folder_path = os.path.join(root_folder, folder)
    
    # Kontrollera om det är en mapp
    if os.path.isdir(folder_path):
        output_file = os.path.join(root_folder, f"{folder}_ski2.txt")
        
        with open(output_file, "w", encoding="utf-8") as outfile:
            # Loopa igenom varje fil i mappen
            for file in sorted(os.listdir(folder_path)):  # Sortera filerna om ordning är viktig
                file_path = os.path.join(folder_path, file)
                
                # Kontrollera om det är en textfil
                if os.path.isfile(file_path) and file.endswith(".txt"):
                    with open(file_path, "r", encoding="utf-8") as infile:
                        # Läs rad för rad och ta bort eventuella extra radbrytningar
                        lines = [line.rstrip() for line in infile if line.strip()]
                        outfile.write("\n".join(lines) + "\n")  # Skriv utan extra radbrytningar

print("Sammanställning klar!")
