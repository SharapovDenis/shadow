import json
import ijson
import requests
import sqlite3
from tqdm import tqdm

def file_reader(f):
    for s in ijson.items(f, 'elements.item'):
        yield s
    

def map_receive(url: str, query: str, save_to: str):
    
    # # Create target Directory if don't exist
    # if not os.path.exists(dirName):
    #     os.mkdir(dirName)
    #     print("Directory " , dirName ,  " Created ")
    # else:    
    #     print("Directory " , dirName ,  " already exists")
    
    response = requests.get(url, params={'data': query})
    print("Response is received. Ready to write a file")
    with open(save_to, "w") as outfile:
        for line in tqdm(response.iter_lines(chunk_size=4096, decode_unicode=True)):
            print(line, file=outfile)
    outfile.close()
    
def db_create(db_name :str, query_file: str):
    con = sqlite3.connect(db_name)
    cur = con.cursor()
    
    q_file = open(query_file, 'r')
    query = q_file.read()
    q_file.close()
    
    cur.executescript(query)
    
    con.commit()
    cur.close()
    con.close()

def db_insert(db_name: str, json_file: str):
    j_file = open(json_file, 'r')
    
    json_data = file_reader(j_file)

    con = sqlite3.connect(db_name)
    cur = con.cursor()
    
    for x in tqdm(json_data):
        if(x["type"] == "node"):
            node_id = int(x["id"])
            lat     = float(x["lat"])
            lon     = float(x["lon"])
            cur.execute("INSERT INTO nodes VALUES (?, ?, ?)", (node_id, lat, lon))
            if('tags' in x.keys()):
                for k in x['tags'].keys():
                    tag_key = k
                    tag_val = x['tags'][k]
                    cur.execute("INSERT INTO node_tags VALUES (?, ?, ?)", (node_id, tag_key, tag_val))
        if(x["type"] == "way"):
            way_id = int(x["id"])
            if('tags' in x.keys()):
                for k in x['tags'].keys():
                    tag_key = k
                    tag_val = x['tags'][k]
                    cur.execute("INSERT INTO way_tags VALUES (?, ?, ?)", (way_id, tag_key, tag_val))
            if('nodes' in x.keys()):
                for j in range(len(x['nodes'])):
                    node_id = int(x['nodes'][j])
                    cur.execute("INSERT INTO ways VALUES (?, ?, ?)", (way_id, node_id, j))
        if(x["type"] == "relation"):
            rel_id = int(x["id"])
            if('tags' in x.keys()):
                for k in x['tags'].keys():
                    tag_key = k
                    tag_val = x['tags'][k]
                    cur.execute("INSERT INTO rel_tags VALUES (?, ?, ?)", (rel_id, tag_key, tag_val))
            if('members' in x.keys()):
                for j in range(len(x['members'])):
                    member_type = x['members'][j]['type']
                    member_id = int(x['members'][j]['ref'])
                    member_role = x['members'][j]['role']
                    cur.execute("INSERT INTO relations VALUES (?, ?, ?, ?, ?)", (rel_id, member_type, member_id, member_role, j))
    
    j_file.close()
    con.commit()
    cur.close()
    con.close()
