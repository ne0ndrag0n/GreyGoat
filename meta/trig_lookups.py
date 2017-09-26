import math

output = "const fix32 tanLookup[ 1921 ] = { "

for i in range( 0, 1921 ):
    result = math.tan( math.radians( i * 0.1875 ) )
    result = '{:.3f}'.format( result )

    if result == '0.000' or result == '-0.000':
        result = '0.001'

    output += 'FIX32({})'.format( result )

    if i != 1920:
        output += ", "

output += " };"

outfile = open( "result.c", "w" )
outfile.write( output )
outfile.close()
