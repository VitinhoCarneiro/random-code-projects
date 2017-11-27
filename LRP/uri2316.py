#!/usr/bin/env python3

def merge_sort(elements, reverse=False, key=lambda i: i, comp=lambda i, j: i > j):
	if len(elements) > 2:
		list1 = merge_sort(elements[:int(len(elements) / 2)], reverse=reverse, key=key, comp=comp)
		list2 = merge_sort(elements[int(len(elements) / 2):], reverse=reverse, key=key, comp=comp)
		result = merge(list1, list2, reverse=reverse, key=key, comp=comp)
		return result
	else:
		if len(elements) < 2: return elements
		if(comp(key(elements[0]), key(elements[1])) ^ reverse): elements = elements[::-1]
		return elements

def merge(list1, list2, reverse=False, key=lambda i: i, comp=lambda i, j: i > j):
	result = []
	while(len(list1) > 0 and len(list2) > 0):
		if (comp(key(list1[0]), key(list2[0]))) ^ reverse: result.append(list2.pop(0))
		else: result.append(list1.pop(0))
	if(len(list1) > 0): result.extend(list1)
	else: result.extend(list2)
	return result

def carcmp(c1, c2):
	if not (c1["sense_count"] == c2["sense_count"]): return c1["sense_count"] < c2["sense_count"]
	else: return c1["last_sense_tstamp"] > c2["last_sense_tstamp"]

(num_sensors, num_cars, num_readings) = (list(map(int, input().split())))
car_states = [{"car": i, "lap": 0, "last_sensor": num_sensors - 1, "sense_count": 0, "last_sense_tstamp": 0} for i in range(num_cars)]
for i in range(num_readings):
	(car, sensor) = list(map(lambda i: int(i) - 1, input().split()))
	if((sensor - 1) % num_sensors == car_states[car]["last_sensor"]):
		car_states[car]["last_sensor"] = sensor
		car_states[car]["sense_count"] += 1
		car_states[car]["last_sense_tstamp"] = i
end_car_states = merge_sort(car_states, comp=carcmp)
#print(car_states)
#print(end_car_states)
print(' '.join(map(lambda i: str(i["car"] + 1), end_car_states)))

'''
(num_sensors, num_cars, num_readings) = list(map(int, input().split()))
readings = [[num_sensors - 1, 0, 0, i] for i in range(num_cars)]
for i in range(num_readings):
	(car, sensor) = list(map(lambda i: int(i) - 1, input().split()))
	if(sensor - 1 % num_sensors == readings[car][0]):
		readings[car][0] = sensor
		readings[car][1] += 1
		readings[car][2] = i
readings = merge_sort(readings, comp=carcmp)
print(' '.join(map(lambda i: str(i[3] + 1), readings)))
'''
