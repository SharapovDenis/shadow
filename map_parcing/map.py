import overpy
import pandas

coordinates = "55.7263,37.6503,55.7816,37.7864"

api = overpy.Overpass()
result = api.query(f"""
    [out: json];
    wr[building]({coordinates})(if:number(t["building:levels"]) >= 1);
    out geom;
                   
""")

table_of_ways = []

# проблема с оперативкой может быть, т. к. может быть слишком большой граф
for way in result.ways:
    list_of_ways = [0]*5
    list_of_ways[0] = way.id
    list_of_ways[1] = way.tags.get("addr:city")
    list_of_ways[2] = way.tags.get("addr:street")
    list_of_ways[3] = way.tags.get("addr:housenumber")
    list_of_ways[4] = way.tags.get("building:levels")
    table_of_ways.append(list_of_ways)  
    
df = pandas.DataFrame(table_of_ways, columns=["id", "city", "street", "housenumber", "levels"])
df.to_csv("output_data.csv")
df.to_excel("output_data.xlsx")
