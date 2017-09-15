import random

output = "const u8 randLookup[ 40000 ] = { "

for i in range( 0, 40000 ):
    output += str( random.randint( 0, 223 ) )

    if i != 39999:
        output += ", "

output += " };"

outfile = open( "result.c", "w" )
outfile.write( output )
outfile.close()
