#!/bin/bash

import os

backends=('helide', 'barney', 'visrtx', 'ospray')

os.system('mkdir backends')
with open('backends/index.html','w') as f:

    f.write("<html><body>\n")
    f.write('<table rules="all">\n')
    f.write('<tr>\n')
    for backend in backends:
        f.write(f'<th>{backend}</th>\n')
    f.write('</tr>\n')
    for id in range(1,7):
        #f.write('<p><br>\n')
        f.write('<tr>\n')
        #        f.write('<div>\n')
        for backend in backends:
            print(f'=== running sample0{id} on backend {backend}')
            cmd = f"ANARI_LIBRARY={backend} python3 sample0{id}.py -o backends/{backend}_sample0{id}.png"
            os.system(cmd)
            cmd = f"convert backends/{backend}_sample0{id}.png backends/{backend}_sample0{id}.jpg"
            os.system(cmd)
            f.write(f'<td><img src="{backend}_sample0{id}.jpg" width="100%"></td>\n')
            #f.write('</div>\n')
        f.write('</tr>\n')
    f.write('</table>\n')
    f.write('</body></html>\n')
os.system('chmod a+rX backends -R')
