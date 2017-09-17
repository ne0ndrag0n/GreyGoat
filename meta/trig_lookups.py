import math

output = "const fix16 tanLookup[ 360 ] = { "

for i in range( 0, 360 ):
    output += 'FIX16({})'.format( str( math.tan( math.radians( i ) ) ) )

    if i != 359:
        output += ", "

output += " };"

outfile = open( "result.c", "w" )
outfile.write( output )
outfile.close()
