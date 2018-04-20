python conda create -n chinese-ocr python=3.6 pip scipy numpy pillow
source activate chinese-ocr
pip3 install easydict -i https://pypi.tuna.tsinghua.edu.cn/simple/
pip3 install keras==2.0.8  -i https://pypi.tuna.tsinghua.edu.cn/simple/  
pip3 install Cython opencv-python -i https://pypi.tuna.tsinghua.edu.cn/simple/ 
pip3 install matplotlib -i https://pypi.tuna.tsinghua.edu.cn/simple/ 
pip3 install -U pillow -i https://pypi.tuna.tsinghua.edu.cn/simple/
pip3 install h5py lmdb mahotas -i https://pypi.tuna.tsinghua.edu.cn/simple/
python conda install pytorch=0.1.12 torchvision -c soumith
python conda install tensorflow=1.4 
cd ./ctpn/lib/utils
cython bbox.pyx
cython cython_nms.pyx
python setup_cpu.py build_ext --inplace
move utils/* ./
del build
del utils

