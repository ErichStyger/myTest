# myTest
Repo used for testing CI/CD with GitHub

- how to create a GitHub action:
  https://www.incredibuild.com/blog/using-github-actions-with-your-c-project
  https://github.blog/2021-11-04-10-github-actions-resources-basics-ci-cd/
- solving 'permission denied' on config file:
  \href{https://dev.to/aileenr/github-actions-fixing-the-permission-denied-error-for-shell-scripts-4gbl}{permission denied}
- Unity: https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityGettingStartedGuide.md
- Docker for RP2040:
https://www.digikey.ch/en/maker/projects/continuous-deployment-using-docker-and-github-actions/d9d18e19361647dbb49070ce6f96c2ea
- Getting started with Docker:
https://www.digikey.ch/en/maker/projects/getting-started-with-docker/aa0d4c708c274ffd975f3b427e5c0ce6

# Docker
- Installed Docker Desktop
- To run it on a local docker, cd to Dockerfile:
- build image with the Dockerfile:
docker build -t pico-builder-image .

- reate Container from the Image:
docker create --name pico-builder-container pico-builder-image
- copy file out of container
docker cp pico-builder-container:/project/src/build/blink.uf2 ./blink.uf2

- REMOVE the container and image
docker rm pico-builder-container
docker rmi pico-builder-image