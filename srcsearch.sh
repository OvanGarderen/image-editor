for f in src/*
do 
   cat -n "$f" | grep "$1" && echo -e "\033[31min $f\033[0m"
done
