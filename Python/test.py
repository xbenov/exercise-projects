def list_comp():
    nums = [1,2,3,4,5,6,7,8,9,10]

    my_list = []
    for n in nums:
        my_list.append(n)
    print(my_list)

    my_list = [n for n in nums]
    print(my_list)

    my_list = [n*n for n in nums]
    print(my_list)

    my_list = [n for n in nums if n%2 == 0]
    print(my_list)

    my_list = [(letter,num) for letter in 'abcd' for num in range(4)]
    print(my_list)

def ex1(n1,n2):
    '''Given two integer numbers, return their product if it is lower or equal to 1000, else return their sum.'''
    return n1*n2 if n1*n2 <= 1000 else n1+n2 

#print(ex1(20,30))
#print(ex1(40,30))

def ex2():
    '''Iterate 10 numbers and print the some of current and previous number.'''
    out = "Current-> {} | Previous-> {} | Sum-> {}"
    for n in range(10):
        print(out.format(n, n if n<1 else n-1,n+ (n if n<1 else n-1)))

#ex2()

def ex3():
    '''Accept a string and print only characters at even index.'''
    str = input('Enter input: ')
    for i,c in enumerate(str):
        if i%2 == 0:
            print(c)

#ex3()

def ex4(str,n):
    '''Removes first n characters from a string'''
    if n >= len(str):
        return "n must be less than string length"
    return str[n:]

#print(ex4("pynative",3))

def ex5():
    '''Check if first and lats n in list is the same'''
    