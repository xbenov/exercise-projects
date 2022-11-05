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
    '''
    Given two integer numbers, return their product if it is lower or equal to 1000, else return their sum.
    '''
    return n1*n2 if n1*n2 <= 1000 else n1+n2 

#print(ex1(20,30))
#print(ex1(40,30))

def ex2():
    '''
    Iterate 10 numbers and print the some of current and previous number.
    '''
    out = "Current-> {} | Previous-> {} | Sum-> {}"
    for n in range(10):
        print(f"Current-> {n} | Previous-> {n if n<1 else n-1} | Sum-> {n+ (n if n<1 else n-1)}")

#ex2()

def ex3():
    '''
    Accept a string and print only characters at even index.
    '''
    str = input('Enter input: ')
    for i,c in enumerate(str):
        if i%2 == 0:
            print(c)

#ex3()

def ex4(str,n):
    '''
    Removes first n characters from a string
    '''
    if n >= len(str):
        return "n must be less than string length"
    return str[n:]

#print(ex4("pynative",3))

def ex5(list):
    '''
    Check if first and last n in list is the same
    '''
    if list[0] == list[-1]:
        return True
    else:
        return False

#print(ex5([10,20,30,40,101]))

def ex6(list):
    '''
    Print numbers divisible by 5 from list
    '''
    for n in list:
        if n%5==0:
            print(n)

#ex6([10,20,33,46,55])

def ex7(str,substr):
    '''
    Return count of substring in string
    '''
    count = str.count(substr)
    print(count)

#ex7("Hello world is hello world","world")

def ex8(size):
    '''
    Print a triangle
    '''
    for x in range(size+1):
        for y in range(x):
            print(x,end=' ')
        print()

#ex8(5)

def ex9(n):
    '''
    Check if number is palindrome
    '''
    rev_n = 0
    o_n = n

    while n > 0:
        rev_n *= 10
        rev_n += n%10
        n = n//10
    
    if o_n == rev_n:
        print(f"Number {o_n} is palindrome")
    else:
        print(f"Number {o_n} is not palindrome")

#ex9(125)

def ex10(list_o,list_e):
    '''
    Create new list containing odd numbers from one list and ecen from another list
    '''
    odds = [n for n in list_o if n%2 == 1]
    evens = [n for n in list_e if n%2 == 0]
    print(odds + evens)

#ex10([10,20,25,30,35],[40,45,60,75,90])
        
def ex11(n):
    '''
    Print digits of int number in reverse
    '''
    l = []
    while(n > 0):
        l.append(str(n%10))
        n//=10

    string = ' '.join(l)

    return string

def ex11b(n):
    return ' '.join(str(n)[::-1])

#print(ex11b(123456))

def ex12(income):
    '''
    Income tax
    '''
    if income <= 10000:
        return 0
    elif (income-10000) <= 10000:
        return (income-10000)*0.1
    else:
        return 1000 + (income - 20000)*0.2

#print(ex12(45000))

def ex13():
    '''
    Print multiplication table
    '''
    for n in range(1,11):
        for m in range(1,11):
            print(f'{n*m:02}',end=' ')
        print('\n')

#ex13()

def ex14(size):
    '''
    Downward triangle using asterisks
    '''
    for n in range(size):
        for m in range(size-n):
            print('*',end=' ')
        print('\n')

#ex14(4)

def ex15(base: int,exp: int):
    '''
    Calculate exponential value
    '''
    return base**exp

print(ex15(5,-1))