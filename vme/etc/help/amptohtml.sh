#bash

# make sure these two characters are unique in all zon files
# e.g. run grep "\^\^" *.zon to check that ^^ are unique
#

# First zap strings that we wont use (&l clashes with &lt;)
sed -i -- 's/&l//g' strings/*
sed -i -- 's/&f//g' strings/*

# Remove all colors
sed -i -- 's/&cw//g' strings/*
sed -i -- 's/&cr//g' strings/*
sed -i -- 's/&cy//g' strings/*
sed -i -- 's/&cg//g' strings/*
sed -i -- 's/&cb//g' strings/*
sed -i -- 's/&cm//g' strings/*
sed -i -- 's/&cc//g' strings/*
sed -i -- 's/&cn//g' strings/*

sed -i -- 's/&c+w//g' strings/*
sed -i -- 's/&c+r//g' strings/*
sed -i -- 's/&c+y//g' strings/*
sed -i -- 's/&c+g//g' strings/*
sed -i -- 's/&c+b//g' strings/*
sed -i -- 's/&c+m//g' strings/*
sed -i -- 's/&c+c//g' strings/*
sed -i -- 's/&c+n//g' strings/*

# Remove &sn and &snn 
sed -i -- 's/&s[0-9][0-9]/ /g' strings/*
sed -i -- 's/&s[0-9]/ /g' strings/*

#Change & to &amp;
# Need to do this before we get &gt; &lt;

sed -i -- 's/&&/\&amp;/g' strings/*

# Important to do before inserting HTML codes
# Change > to &gt; and < to &lt;
sed -i -- 's/>/\&gt;/g' strings/*
sed -i -- 's/</\&lt;/g' strings/*


# Now we know no files contain < or >
# Lets replace all newlines \n with <br/>\n
#
for f in strings/*
do
  # replace \n with < in new file .nl
  cat $f | tr '\n' '<' > $f.nl
  mv $f.nl $f                   # move the file back and overwrite original
done

sed -i -- 's/</<br\/>\n/g' strings/*  # change < to <br/>\n

#
#
sed -i -- 's/&n/\<br\/>/g' strings/*

# remove &[default]
sed -i -- 's/&\[default\]//g' strings/*
