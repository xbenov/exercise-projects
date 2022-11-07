def ex1():
    '''
    Multiply two numbers fdrom user input
    '''
    a = input('Input first number: ')
    b = input('Input second number: ')
    print(f'{a} * {b} = {int(a)*int(b)}')

#ex1()

def ex2():
    '''
    Print string with ** separator
    '''
    print('Name','Is','Glaedr',sep='**')

#ex2()

def ex3(n):
    '''
    Convert number using print
    '''
    print(f'The octal number of decimal number {n} is {n:o}')

#ex3(8)

def ex4(n=3.141):
    '''
    Float with 2 dec. places
    '''
    print(f'The number is {n:.2f}')

#ex4()

def ex5():
    '''
    Input a list of 5 floats#
    '''
    print([float(input('Input a number: ')) for x in range(5)])

#ex5()

def ex6():
    '''
    Copy test file contents without the 5th line
    '''
    with open(r'E:\Programming\exercise-projects\Python\practice\code\test.txt','r') as filein:
        with open(r'E:\Programming\exercise-projects\Python\practice\code\test_copy.txt','w') as fileout:
            for index,line in enumerate(filein):
                if index == 4:
                    continue
                fileout.write(line)

#ex6()

def ex7():
    '''
    Accept 3 strings from one input call
    '''
    names = input('Enter three names: ').split()
    print(names)

#ex7()

def ex8():
    '''
    Format variables using string format method
    '''
    totalMoney = 1000
    quantity = 3
    price = 450

    print("I have {} dollars so I can buy {} football for {:.2f} dollars.".format(totalMoney,quantity,price))

#ex8()
import os
def ex9(path):
    '''
    Check if file is empty
    '''
    stats = os.stat(path)
    if stats.st_size > 0:
        print('File is not empty')
    else:
        print('File is empty')

#ex9(r'E:\Programming\exercise-projects\Python\practice\code\empty.txt')

def ex10():
    '''
    Read 4th line from file
    '''
    with open(r'E:\Programming\exercise-projects\Python\practice\code\test.txt','r') as file:
        print(file.readlines()[2],end='')

ex10()


