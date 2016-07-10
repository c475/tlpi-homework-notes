# no way am i doing this one in C



############### CREATE A NEW FILE ###############

mkdir testdir

# remove all but write perms on testdir
chmod -x,-r testdir
touch testdir/coolfile.txt
# touch: cannot touch ‘testdir/cool’: Permission denied
chmod +x testdir
# needs +x,+w on directory to create a file.
# file does not need any permissions to be created...
touch testdir/coolfile.txt


############### OPEN FILE FOR READING ###############

mkdir testdir
touch testdir/coolfile.txt

echo "TEST TEST" >> testdir/coolfile.txt

chmod +r,-w,-x testdir/coolfile.txt
chmod -r,-w,+x testdir
# onle need execute permission on dir to read the file
# only read permission needed on file
cat testdir/coolfile.txt


############### OPEN FILE FOR WRITING ###############

mkdir testdir
touch testdir/coolfile.txt

# write on file
# execute on directory
chmod -r,+w,-x testdir/coolfile.txt
chmod -r,-w,+x testdir

echo "TEST TEST" >> testdir/coolfile.txt

cat testdir/coolfile.txt


############### DELETE FILE ###############

mkdir testdir
touch testdir/coolfile.txt

# no permissions needed on file
# write and execute needed on dir
chmod -r,-w,-x testdir/coolfile.txt
chmod -r,+w,+x testdir

rm -f testdir/coolfile.txt


################# COME BACK TO THE REST, IT'S A BIT CONFUSING #######################
