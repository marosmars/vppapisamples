# SETUP
# Put these files in your go workspace under folder vppapisample
# go get git.fd.io/govpp.git
# go install git.fd.io/govpp.git/cmd/binapi-generator

# BUILD
go generate
go build
sudo ./vppapisample
