git diff-tree --no-commit-id --name-only -r $1 $2 | while read -r line; 
do
	if [ -f "$line" ]; then
		echo "Traitement du fichier $line ...";  
		if expr match "$line" '\..*' || expr match "$line" '.*\.sh'; then 
			echo "Fichier ignoré."; 
		else 
			modded_path="$(echo $line | sed -E 's@(.*?/)Developers/[^/]*/@\1@g')";
			if [ "$line" != "$modded_path" ]; then
				modded_path=$(echo $modded_path | sed 's@/@\%2f@g');
				file=$(basename "$line");
				rm -f ./tmp.json;
				echo '{"branch": "dev", "author_email": "'$mail'", "encoding":"base64", "content":"'$(cat $line | base64)'", "commit_message": "[ci skip] 0d37cd5b - Add '$file'"}' >> tmp.json;
				message=$(curl --request POST --header Private-Token:$4 --header "Content-Type: application/json" --data "@./tmp.json" "https://gitlab.com/api/v4/projects/21878653/repository/files/$modded_path");
				if expr match "$message" '.*exists'; then
					echo "Déjà existant! Mis à jour...";
					message=$(curl --request PUT --header Private-Token:$4 --header "Content-Type: application/json" --data "@./tmp.json" "https://gitlab.com/api/v4/projects/21878653/repository/files/$modded_path");
				fi;
				json='{"branch": "dev", "author_email": "'$3'", "commit_message": "[ci skip] '$2' - Remove old '$file'"}';
				line=$(echo $line | sed 's@/@\%2f@g');
				message=$(curl --request DELETE --header Private-Token:$4 --header "Content-Type: application/json" --data "$(echo $json)" "https://gitlab.com/api/v4/projects/21878653/repository/files/$line");
				echo "Fait."; 
			else 
				echo "Fichier ignoré.";
			fi; 
		fi;
	fi; 
done;
