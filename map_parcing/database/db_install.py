import shadow
import tracemalloc
import time

q_text = """ [out: json][timeout:900][maxsize:2000000000];
(	
  	area[place=city][name="Москва"] -> .moscow;
 	nwr(area.moscow);
);
    out geom;"""
    
url = 'https://overpass.openstreetmap.ru/api/interpreter'

t0 = time.time()

# starting the monitoring
tracemalloc.start()

print(f"Receiving data from {url}")
shadow.map_receive(url, q_text, './map/map.json')

print("Creating db")
shadow.db_create('shadow.db', './queries/tables_create.sql')

print("Db created. Time after begining:", time.time() - t0)

print("Inserting data in db")
shadow.db_insert('shadow.db', './map/map.json')

# displaying the memory
print("Memory used:", tracemalloc.get_traced_memory()[1] / (1024**2), "Mb")
 
# stopping the library
tracemalloc.stop()
