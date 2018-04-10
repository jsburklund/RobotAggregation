
f_temp = open("aggregation_test.argos_templ", "r")

templ_string = f_temp.read().format(seed=5)
#val_string = templ_string.format(5)

print(templ_string)
