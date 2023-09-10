# create two files root.txt and home.txt inside two separate folders
# before creation date and sleep 3
date
sleep 3
mkdir root
date
sleep 3
mkdir home
touch root.txt
date
sleep 3
touch home.txt
mv root.txt ~/week01/root
mv home.txt ~/week01/home
# ls files from directories sorted
ls / -t -r > ~/week01/root/root.txt
ls ~ -t -r > ~/week01/home/home.txt
# print items
echo "---ROOT---"
cat < ~/week01/root/root.txt
echo "---HOME---"
cat < ~/week01/home/home.txt
echo "---week01/root, week01/home---"
ls ~/week01/home
ls ~/week01/root
