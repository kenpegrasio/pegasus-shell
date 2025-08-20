[![progress-banner](https://backend.codecrafters.io/progress/shell/f798d551-5857-4899-aa22-3829493fb4b7)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

This is my C++ solutions to the
["Build Your Own Shell" Challenge](https://app.codecrafters.io/courses/shell/overview).

# Setup Instruction (WSL)

## 1. Install `vcpkg`

```bash
wget -qO vcpkg.tar.gz https://github.com/microsoft/vcpkg/archive/master.tar.gz
sudo mkdir /opt/vcpkg
sudo tar xf vcpkg.tar.gz --strip-components=1 -C /opt/vcpkg
sudo /opt/vcpkg/bootstrap-vcpkg.sh
sudo ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg
rm -rf vcpkg.tar.gz
```

## 2. Set `VCPKG_ROOT` in the environment

```bash
echo 'export VCPKG_ROOT=/opt/vcpkg' | sudo tee -a /etc/profile.d/vcpkg.sh
source /etc/profile.d/vcpkg.sh
sudo chown -R $USER:$USER /opt/vcpkg
```

## 3. Convert .sh to Unix line endings

If `dos2unix` is not installed, run this following command

```bash
sudo apt update
sudo apt install -y dos2unix
```

Afterwards, run this command!

```bash
dos2unix your_program.sh
```

## 4. Install essential package

```bash
sudo apt update
sudo apt install -y build-essential
```

## 5. Run

```bash
./your_program.sh
```